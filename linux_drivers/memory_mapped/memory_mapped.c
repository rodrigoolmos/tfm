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
#include "../common/common.h"

void send_64b(void *addr, uint32_t offset, uint64_t *value){

    int i;
    void *virt_addr;

    virt_addr = addr + offset;
    *((uint32_t *) virt_addr) = *value;
    virt_addr = addr + offset + sizeof(uint32_t);
    *((uint32_t *) virt_addr) = *value >> 32;
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

