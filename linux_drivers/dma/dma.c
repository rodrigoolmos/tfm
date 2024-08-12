#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "defines_dma.h"
#include "../common/common.h"

void write_to_burst(int fd, off_t base_addr, const void *data, size_t size) {
    if (pwrite(fd, data, size, base_addr) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void read_from_burst(int fd, off_t base_addr, void *buffer, size_t size) {
    if (pread(fd, buffer, size, base_addr) == -1) {
        perror("pread");
        exit(EXIT_FAILURE);
    }
}

void send_trees(int fd_h2c, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS]){

    int i;
    uint64_t offset;

    for (i = 0; i < N_TREES; i++){
        offset =  N_NODE_AND_LEAFS * sizeof(uint64_t) * i;
        write_to_burst(fd_h2c, TREES_ADDR + offset, tree_data[i], N_NODE_AND_LEAFS * sizeof(uint64_t));
    }

}

void send_features(int fd_h2c, const void *data, uint32_t features_length){
    write_to_burst(fd_h2c, FEATURES_ADDR, data, features_length * sizeof(uint64_t));
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

void read_prediction(void *map_base, int32_t *data){

    void *virt_addr;

    virt_addr = map_base + BASE_ADDRES + PREDICTION_ADDRES;
    *data = *((int32_t *) virt_addr);
    
}

void perform_inference(void *map_base, uint32_t features[N_FEATURE], int fd_h2c, 
                        uint32_t features_length, int32_t *prediction){

    int i;
    uint64_t temp_features[N_FEATURE];

    for (i = 0; i < features_length; i++)
    {
        temp_features[i] = features[i];
    }
                            

    send_features(fd_h2c, temp_features, features_length);
    
    start_prediction(map_base);
    wait_done(map_base);
        
    read_prediction(map_base, prediction);

}

void evaluate_model_hardware(void *map_base, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS], 
                            uint32_t n_samples, struct feature features[MAX_TEST_SAMPLES],
                            uint32_t features_length, int fd_h2c){

    uint32_t status, i;
    clock_t start_time, end_time;
    double cpu_time_used;
    int accuracy = 0;
    int32_t prediction;
    
    send_trees(fd_h2c, tree_data);

    accuracy = 0;
    start_time = clock();
    for ( i = 0; i < n_samples; i++){

        perform_inference(map_base, features[i].features, fd_h2c, features_length, &prediction);
        
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

    int fd_h2c = open("/dev/xdma0_h2c_0", O_RDWR);
    if (fd_h2c == -1) {
        perror("open /dev/xdma0_h2c_0");
        return EXIT_FAILURE;
    }

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

    printf("Executing HW with DMA !!!!!!!!!!\n");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/diabetes.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c);

    read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/heart_attack.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c);

    read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/lung_cancer.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c);                      

    read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/anemia.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c);

    read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/alzheimers.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c);

    close(fd_h2c);
    
    return 0;
}