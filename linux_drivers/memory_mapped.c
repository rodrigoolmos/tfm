#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include "defines_memoty_mapped.h"

#define MAX_LINE_LENGTH 1024
#define MAX_TEST_SAMPLES 3000



#define N_NODE_AND_LEAFS 256
#define N_TREES 128
#define N_FEATURE 32

struct feature {
    float features[N_FEATURE];
    uint8_t prediction;
};

typedef union {
    float f;
    int32_t i;
} float_int_union;

struct tree_camps {
    uint8_t leaf_or_node;
    uint8_t feature_index;
    uint8_t next_node_right_index;
    uint8_t padding;
    float_int_union float_int_union;
};

typedef union {
    struct tree_camps tree_camps;
    uint64_t compact_data;
} tree_data;

void send_64b(void *addr, uint32_t offset, uint64_t *value){

    int i;
    void *virt_addr;

    virt_addr = addr + offset;
    *((uint32_t *) virt_addr) = *value;
    virt_addr = addr + offset + sizeof(uint32_t);
    *((uint32_t *) virt_addr) = *value >> 32;
}

void load_model(
            tree_data tree_data[N_TREES][N_NODE_AND_LEAFS],
            const char *filename) {

    char magic_number[5] = {0};
    int t, n;
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening the file\n");
        return;
    }

    fread(magic_number, 5, 1, file);

    if (!memcmp(magic_number, "model", 5)){
        for (t = 0; t < N_TREES; t++) {
            for (n = 0; n < N_NODE_AND_LEAFS; n++) {
                fread(&tree_data[t][n], sizeof(uint64_t), 1, file);
            }
        }
    }else{
        perror("Unknown file type");
    }

    fclose(file);
}


int read_n_features(const char *csv_file, int n, struct feature *features, uint32_t *features_length) {
    FILE *file = fopen(csv_file, "r");
    char line[MAX_LINE_LENGTH];
    int features_read = 0;
    int index;
    int i;

    if (!file) {
        perror("Failed to open the file");
        return -1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) && features_read < n) {
        float temp[N_FEATURE + 1];
        char *token = strtok(line, ",");
        index = 0;

        while (token != NULL && index < N_FEATURE + 1) {
            temp[index] = atof(token);
            token = strtok(NULL, ",");
            index++;
        }

        for (i = 0; i < index - 1; i++) {
            features[features_read].features[i] = temp[i];
        }
        features[features_read].prediction = (uint8_t) temp[index - 1];

        features_read++;
    }

    *features_length = index;

    fclose(file);
    return features_read;
}

void read_features(void *map_base, uint32_t data[N_FEATURE]){
    int i;
    void *virt_addr;

    for (i = 0; i < N_FEATURE; i++){
        virt_addr = map_base + BASE_ADDRES + FEATURES_ADDRES + sizeof(uint64_t)*i;
        data[i] = *((uint32_t *) virt_addr);
    }
}

void send_features(void *map_base, uint32_t data[N_FEATURE], uint32_t features_length){
    int i;
    void *virt_addr;

    for (i = 0; i < features_length; i++){
        virt_addr = map_base + BASE_ADDRES + FEATURES_ADDRES + sizeof(uint64_t)*i;
        *((uint32_t *) virt_addr) = data[i];
    }
}

void send_trees(void *map_base, uint64_t data[N_TREES][N_NODE_AND_LEAFS]){
    int i, j;
    void *virt_addr;

    for (i = 0; i < N_TREES; i++){
        for (j = 0; j < N_NODE_AND_LEAFS; j++){ 
            virt_addr = map_base + BASE_ADDRES + TREE_ADDR + sizeof(uint64_t) * (i * N_NODE_AND_LEAFS + j);

            send_64b(virt_addr, 0, &data[i][j]);
        }
    }
}

void read_prediction(void *map_base, int32_t *data){

    void *virt_addr;

    virt_addr = map_base + BASE_ADDRES + PREDICTION_ADDRES;
    *data = *((int32_t *) virt_addr);
    
}

void start_prediction(void *map_base){
    
    void *virt_addr;

    virt_addr = map_base + BASE_ADDRES + CONTROL_ADDRES;
    *((uint32_t *) virt_addr) = 0x01;

}

void read_status(void *map_base, uint32_t *data){

    void *virt_addr;

    virt_addr = map_base + BASE_ADDRES + CONTROL_ADDRES;
    *data = *((uint32_t *) virt_addr);
}

void wait_done(void *map_base){

    void *virt_addr;

    uint32_t data;

    while (1){
        virt_addr = map_base + BASE_ADDRES + CONTROL_ADDRES;
        data = *((uint32_t *) virt_addr);

        if (data&0x2)
            break;
    }
}

void perform_inference(void *map_base, uint32_t *features, 
                        uint32_t features_length, uint32_t *prediction){

    send_features(map_base, features, features_length);
    
    start_prediction(map_base);
    wait_done(map_base);
        
    read_prediction(map_base, prediction);

}

void evaluate_model_hardware(void *map_base, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS], 
                            uint32_t n_samples, struct feature features[MAX_TEST_SAMPLES],
                            uint32_t features_length){

    uint32_t status, i;
    clock_t start_time, end_time;
    double cpu_time_used;
    int accuracy = 0;
    int32_t prediction;
    
    send_trees(map_base, tree_data);

    accuracy = 0;
    start_time = clock();
    for ( i = 0; i < n_samples; i++){

        perform_inference(map_base, features[i].features, features_length, &prediction);
        
        if (features[i].prediction == (prediction > 0))
            accuracy++;
    }
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;           
    printf("Tiempo de ejecucion por feature: %f segundos\n", cpu_time_used / n_samples);
    printf("Accuracy %f\n", 1.0 * accuracy / n_samples);
}

int main() {

    int read_samples;
    uint32_t features_length;
    int fd;
    void *map_base;

    tree_data tree_data[N_TREES][N_NODE_AND_LEAFS];
    struct feature features[MAX_TEST_SAMPLES] = {0};

    char *resource_path = "/sys/bus/pci/devices/0000:01:00.0/resource0";
    
    if ((fd = open(resource_path, O_RDWR | O_SYNC)) == -1) {
        perror("Error abriendo el dispositivo PCIe");
        exit(1);
    }

    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map_base == (void *) -1) {
        perror("Error en mmap");
        close(fd);
        exit(1);
    }

    printf("Executing HW\n");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/diabetes.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length);
    
    read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/heart_attack.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length);

    read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/lung_cancer.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length);

    read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/anemia.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length);

    read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/alzheimers.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length);

    return 0;
}

