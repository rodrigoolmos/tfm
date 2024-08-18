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

#define TRUE  1
#define FALSE 0

uint32_t n_model = 0;

void write_axi_lite_32b(void *addr, uint32_t offset, uint32_t *value, uint32_t vector_elemnts){

    int i;
    void *virt_addr;

    for (i = 0; i < vector_elemnts; i++){
        virt_addr = addr + offset + sizeof(uint32_t)*i;
        *((uint32_t *) virt_addr) = value[i];
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

void write_burst(int fd, off_t base_addr, const void *data, size_t size) {
    if (pwrite(fd, data, size, base_addr) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void read_burst(int fd, off_t base_addr, void *buffer, size_t size) {
    if (pread(fd, buffer, size, base_addr) == -1) {
        perror("pread");
        exit(EXIT_FAILURE);
    }
}

void send_trees_to_accelerator(int fd_h2c, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS]){

    int i;
    uint64_t offset;

    for (i = 0; i < N_TREES; i++){
        offset =  N_NODE_AND_LEAFS * sizeof(uint64_t) * i;
        write_burst(fd_h2c, TREES_ADDR + offset, tree_data[i], N_NODE_AND_LEAFS * sizeof(uint64_t));
    }
}

void send_trees_to_DDR3(int fd_h2c, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS]){

    int i;
    uint64_t offset;
    uint32_t model_size =  N_TREES * N_NODE_AND_LEAFS * sizeof(uint64_t);

    for (i = 0; i < N_TREES; i++){
        offset =  N_NODE_AND_LEAFS * sizeof(uint64_t) * i;
        write_burst(fd_h2c, n_model * model_size + offset, 
            tree_data[i], N_NODE_AND_LEAFS * sizeof(uint64_t));
    }
    n_model++;
}

void load_trees_DDR3(void *addr, uint32_t src_addr,  uint32_t dest_addr, uint32_t n_bytes){

    uint32_t busy = 0;

    write_axi_lite_32b(addr, DMA_ADDRES + DMA_DA, &dest_addr, 1);

    write_axi_lite_32b(addr, DMA_ADDRES + DMA_SA, &src_addr, 1);

    write_axi_lite_32b(addr, DMA_ADDRES + DMA_BTT, &n_bytes, 1);

    while (!(busy & 0x02))
        read_axi_lite_32b(addr, DMA_ADDRES + DMA_STATUS, &busy, 1);

}

void send_features(void *map_base, uint32_t data[N_FEATURE], uint32_t features_length){
    int i;
    void *virt_addr;
    float *temp;

    for (i = 0; i < features_length; i++){
        virt_addr = map_base + BASE_ADDRES + FEATURES_ADDR + sizeof(uint64_t)*i;
        *((uint32_t *) virt_addr) = data[i];
        temp = &data[i];

        int stop=0;
    }
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

    send_features(map_base, features, features_length);
    
    start_prediction(map_base);
    wait_done(map_base);
        
    read_prediction(map_base, prediction);

}

void evaluate_model_hardware(void *map_base, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS], 
                            uint32_t n_samples, struct feature features[MAX_TEST_SAMPLES],
                            uint32_t features_length, int fd_h2c, uint8_t stored, 
                            uint32_t n_model){

    uint32_t status, i;
    clock_t start_time, start_time2, end_time;
    double cpu_time_used, cpu_time_used2;
    int accuracy = 0;
    int32_t prediction;
    uint32_t trees_addr = TREES_ADDR;
    uint32_t model_size =  N_TREES * N_NODE_AND_LEAFS * sizeof(uint64_t);
    uint32_t trees_DDR3_addr = n_model * model_size;

    start_time2 = clock();
    if (stored == TRUE){
        load_trees_DDR3(map_base, trees_DDR3_addr, trees_addr, N_TREES * N_NODE_AND_LEAFS * sizeof(uint64_t));
    }else{
        send_trees_to_accelerator(fd_h2c, tree_data);
    }

    start_time = clock();
    accuracy = 0;
    for ( i = 0; i < n_samples; i++){

        perform_inference(map_base, features[i].features, fd_h2c, features_length, &prediction);
        
        if (features[i].prediction == (prediction > 0))
            accuracy++;
    }
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;           
    cpu_time_used2 = ((double)(end_time - start_time2)) / CLOCKS_PER_SEC;           
    printf("Tiempo de ejecucion por feature: %f segundos\n", cpu_time_used / n_samples);
    printf("Tiempo total de ejecucion: %f segundos\n", cpu_time_used2);
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


    printf("Executing HW with DMA with out DDR3!!!!!!!!!!\n");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/diabetes.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, FALSE, 0);

    read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/heart_attack.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, FALSE, 0);

    read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/lung_cancer.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, FALSE, 0);                      

    read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/anemia.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, FALSE, 0);

    read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    load_model(tree_data, "../trained_models/alzheimers.model");
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, FALSE, 0);


    printf("SENDIG MODELS TO DDR3\n");
    load_model(tree_data, "../trained_models/diabetes.model");
    send_trees_to_DDR3(fd_h2c, tree_data);
    load_model(tree_data, "../trained_models/heart_attack.model");
    send_trees_to_DDR3(fd_h2c, tree_data);
    load_model(tree_data, "../trained_models/lung_cancer.model");
    send_trees_to_DDR3(fd_h2c, tree_data);
    load_model(tree_data, "../trained_models/anemia.model");
    send_trees_to_DDR3(fd_h2c, tree_data);
    load_model(tree_data, "../trained_models/alzheimers.model");
    send_trees_to_DDR3(fd_h2c, tree_data);

    printf("Executing HW with DMA with DDR3!!!!!!!!!!\n");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, &features_length);
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, TRUE, 0);
    read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, &features_length);
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, TRUE, 1);
    read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, TRUE, 2);                      
    read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, TRUE, 3);
    read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
    evaluate_model_hardware(map_base, tree_data, read_samples, features, features_length, fd_h2c, TRUE, 4);


    close(fd_h2c);
    
    return 0;
}