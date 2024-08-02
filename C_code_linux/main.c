#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>

#define MAX_LINE_LENGTH 1024
#define MAX_TEST_SAMPLES 3000

#define BASE_ADDRES 0
#define MAP_SIZE 0x70000

#define XPREDICT_COMPACT_DATA_ADDR_COMPACT_DATA_BASE 0x400
#define XPREDICT_FEATURES_ADDR_FEATURES_BASE 0x100
#define XPREDICT_NEXT_NODE_RIGHT_INDEX_ADDR_NEXT_NODE_RIGHT_INDEX_BASE 0x400
#define XPREDICT_NODE_LEAF_VALUE_ADDR_NODE_LEAF_VALUE_BASE 0x1000
#define XPREDICT_PREDICTION_ADDR_PREDICTION_DATA 0x10
#define XPREDICT_RESULTS_ADDR_AP_CTRL 0x0

#define COMPACT_DATA_ADDRES 0x10000 + XPREDICT_COMPACT_DATA_ADDR_COMPACT_DATA_BASE
#define FEATURES_ADDRES 0x20000 + XPREDICT_FEATURES_ADDR_FEATURES_BASE
#define NEXT_NODE_ADDRES 0x30000 + XPREDICT_NEXT_NODE_RIGHT_INDEX_ADDR_NEXT_NODE_RIGHT_INDEX_BASE
#define VALUE_NODE_ADDRES 0x40000 + XPREDICT_NODE_LEAF_VALUE_ADDR_NODE_LEAF_VALUE_BASE
#define PREDICTION_ADDRES 0x50000 + XPREDICT_PREDICTION_ADDR_PREDICTION_DATA
#define RESULT_ADDRES 0x60000 + XPREDICT_RESULTS_ADDR_AP_CTRL

#define N_NODE_AND_LEAFS 64
#define N_TREES 10
#define N_FEATURE 50

struct feature {
    float features[N_FEATURE];
    uint8_t prediction;
};

typedef union {
    float f;
    int32_t i;
} float_int_union;

void wtite_axi_lite_32b(void *addr, uint32_t offset, uint32_t *value, uint32_t vector_elemnts){

    int i;
    void *virt_addr;

    for (i = 0; i < vector_elemnts; i++){
        virt_addr = addr + offset + sizeof(uint32_t)*i;
        *((uint32_t *) virt_addr) = value[i];
    }
}

void wtite_axi_lite_8b(void *addr, uint32_t offset, uint8_t *value, uint32_t vector_elemnts){

    int i;
    void *virt_addr;

    for (i = 0; i < vector_elemnts; i++){
        virt_addr = addr + offset + i;
        *((uint8_t *) virt_addr) = value[i];
    }
}

void read_axi_lite_8b(void *addr, uint32_t offset, uint8_t *value, uint32_t vector_elemnts){

    int i;
    void *virt_addr;

    for (i = 0; i < vector_elemnts; i++){
        virt_addr = addr + offset + sizeof(uint8_t)*i;
        value[i] = *((uint8_t *) virt_addr);
    }

}

void read_axi_lite_32b(void *addr, uint32_t offset, uint32_t *value, uint32_t vector_elemnts){

    int i;
    void *virt_addr;

    for (i = 0; i < vector_elemnts; i++){
        virt_addr = addr + offset + sizeof(uint32_t)*i;
        value[i] = *((uint32_t *) virt_addr);
    }

}

void load_model(float_int_union node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
            uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS],
            uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS],
            const char *filename) {

    int t;

    char magic_number[5] = {0};
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening the file\n");
        return;
    }

    fread(magic_number, 5, 1, file);

    if (!memcmp(magic_number, "model", 5)){
        for (t = 0; t < N_TREES; t++) {
            fread(node_leaf_value[t], sizeof(float), N_NODE_AND_LEAFS, file);
            fread(compact_data[t], sizeof(uint8_t), N_NODE_AND_LEAFS, file);
            fread(next_node_right_index[t], sizeof(uint8_t), N_NODE_AND_LEAFS, file);
        }
    }else{
        perror("Unknown file type");
    }
    
    fclose(file);
}

int read_n_features(const char *csv_file, int n, struct feature *features) {
    FILE *file = fopen(csv_file, "r");
    char line[MAX_LINE_LENGTH];
    int features_read = 0;
    int i;

    if (!file) {
        perror("Failed to open the file");
        return -1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) && features_read < n) {
        float temp[N_FEATURE + 1];
        char *token = strtok(line, ",");
        int index = 0;

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

    fclose(file);
    return features_read;
}

void read_features(void *map_base, uint32_t data[N_FEATURE]){
    read_axi_lite_32b(map_base, BASE_ADDRES + FEATURES_ADDRES, data, N_FEATURE);
}

void send_features(void *map_base, uint32_t data[N_FEATURE]){
    wtite_axi_lite_32b(map_base, BASE_ADDRES + FEATURES_ADDRES, data, N_FEATURE);
}

void read_prediction(void *map_base, uint8_t *data){

    read_axi_lite_8b(map_base, BASE_ADDRES + PREDICTION_ADDRES, data, 1);
    
}

void send_node_leaf_value(void *map_base, uint32_t data[N_TREES][N_NODE_AND_LEAFS]){

    int i;

    for (i = 0; i < N_TREES; i++)
        wtite_axi_lite_32b(map_base, BASE_ADDRES + VALUE_NODE_ADDRES + 
                        i * N_NODE_AND_LEAFS * sizeof(uint32_t), data[i], N_NODE_AND_LEAFS);
    
}
void send_compact_data(void *map_base, uint8_t data[N_TREES][N_NODE_AND_LEAFS]){

    int i;

    for (i = 0; i < N_TREES; i++)
        wtite_axi_lite_8b(map_base, BASE_ADDRES + COMPACT_DATA_ADDRES + 
                        i * N_NODE_AND_LEAFS, data[i], N_NODE_AND_LEAFS);
}
void send_next_node_right_index(void *map_base, uint8_t data[N_TREES][N_NODE_AND_LEAFS]){
    
    int i;

    for (i = 0; i < N_TREES; i++)
        wtite_axi_lite_8b(map_base, BASE_ADDRES + NEXT_NODE_ADDRES + 
                        i * N_NODE_AND_LEAFS, data[i], N_NODE_AND_LEAFS);
}

void start_prediction(void *map_base){
    
    uint32_t start = 0x01;

    wtite_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES, &start, 1);
}
int main() {

    int writeval, control, i, j;
    uint8_t prediction;
    int accuracy = 0;
    int read_samples;
    int fd;
    void *map_base;
    int done = 0;
    float_int_union node_leaf_value[N_TREES][N_NODE_AND_LEAFS];
    uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS];
    uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS];
    struct feature features[MAX_TEST_SAMPLES] = {0};
    float readed_features[N_FEATURE] = {0};

    char *resource_path = "/sys/bus/pci/devices/0000:03:00.0/resource0";
    
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


    load_model(node_leaf_value, compact_data, next_node_right_index, "../trained_models/diabetes.model");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features);

    send_node_leaf_value(map_base, node_leaf_value);
    send_compact_data(map_base, compact_data);
    send_next_node_right_index(map_base, next_node_right_index);

    read_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES,  &control, 1);
    accuracy = 0;

    for ( i = 0; i < read_samples; i++){
        send_features(map_base, features[i].features);
        read_features(map_base, readed_features);
        start_prediction(map_base);
        done = 0;
        while(!done){
            read_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES,  &control, 1);
            if (control&0x2)
            {
                done = 1;
            }
            
        }
        read_prediction(map_base, &prediction);
        if (features[i].prediction == prediction)
            accuracy++;
    }

    printf("Accuracy %f\n", 1.0 * accuracy / read_samples);

    load_model(node_leaf_value, compact_data, next_node_right_index, "../trained_models/heart_attack.model");
    read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features);

    send_node_leaf_value(map_base, node_leaf_value);
    send_compact_data(map_base, compact_data);
    send_next_node_right_index(map_base, next_node_right_index);

    read_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES,  &control, 1);
    accuracy = 0;
    
    for ( i = 0; i < read_samples; i++){
        send_features(map_base, features[i].features);
        read_features(map_base, readed_features);
        start_prediction(map_base);
        done = 0;
        while(!done){
            read_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES,  &control, 1);
            if (control&0x2)
            {
                done = 1;
            }
            
        }
        read_prediction(map_base, &prediction);
        if (features[i].prediction == prediction)
            accuracy++;
    }

    printf("Accuracy %f\n", 1.0 * accuracy / read_samples);

    writeval = 6;
    /*
    wtite_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES, &writeval, 1);
    */
    read_axi_lite_32b(map_base, BASE_ADDRES + RESULT_ADDRES,  &control, 1);


    return 0;
}

