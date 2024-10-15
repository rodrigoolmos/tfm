#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "defines_burst.h"
#include "../common/common.h"

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

void send_features_ping_pong(int fd_h2c, uint32_t* features, uint32_t n_features, uint8_t ping_pong){
    
    if (ping_pong)
        write_burst(fd_h2c, FEATURES_PING_ADDR, features, n_features * N_FEATURE * sizeof(uint32_t));
    else
        write_burst(fd_h2c, FEATURES_PONG_ADDR, features, n_features * N_FEATURE * sizeof(uint32_t));
}

void read_prediction_ping_pong(int fd_c2h, int32_t* predictions, uint32_t n_predictions, uint8_t ping_pong){
    
    if (ping_pong)
        read_burst(fd_c2h, PREDICTIONS_PING_ADDR, predictions, n_predictions * sizeof(uint32_t));
    else
        read_burst(fd_c2h, PREDICTIONS_PONG_ADDR, predictions, n_predictions * sizeof(uint32_t));
}

void send_trees(int fd_h2c, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS]){

    int i;
    uint64_t offset;

    for (i = 0; i < N_TREES; i++){
        offset =  N_NODE_AND_LEAFS * sizeof(uint64_t) * i;
        write_burst(fd_h2c, TREES_ADDR + offset, tree_data[i], N_NODE_AND_LEAFS * sizeof(uint64_t));
    }

}

void read_status(void *map_base, uint32_t *data){

    void *virt_addr;

    virt_addr = map_base + CONTROL_ADDR;
    *data = *((uint32_t *) virt_addr);
}

void start_prediction(void *map_base){
    
    void *virt_addr;

    virt_addr = map_base + CONTROL_ADDR;
    *((uint32_t *) virt_addr) = 0x01;

}

void wait_done(void *map_base){

    void *virt_addr;
    uint32_t data;

    while (1){
        virt_addr = map_base + CONTROL_ADDR;
        data = *((uint32_t *) virt_addr);

        if (data&0x2)
            break;
    }
}

void set_burst_len(void *map_base, uint32_t burst_len){
    
    void *virt_addr;

    virt_addr = map_base + BURST_LENGTH_ADDR;
    *((uint32_t *) virt_addr) = burst_len;

}

void set_ping_pong(void *map_base, uint32_t ping_pong){
    
    void *virt_addr;

    virt_addr = map_base + PING_PONG_ADDR;
    *((uint32_t *) virt_addr) = ping_pong;

}

void coppy_features_to_matrix(struct feature* features, 
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], uint32_t features_length){


    for (uint32_t i = 0; i < features_length; i++)
        memcpy(raw_features[i], features[i].features, sizeof(uint32_t)*N_FEATURE);    

}

int main() {

    int read_samples, i, correct;
    uint32_t status;
    uint32_t features_length;
    
    int fd;
    void *map_base;

    tree_data tree_data[N_TREES][N_NODE_AND_LEAFS];
    struct feature features[MAX_TEST_SAMPLES] = {0};
    uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE] = {0};
    int32_t inference[MAX_TEST_SAMPLES] = {0};

    int fd_h2c = open("/dev/xdma0_h2c_0", O_RDWR);
    if (fd_h2c == -1) {
        perror("open /dev/xdma0_h2c_0");
        return EXIT_FAILURE;
    }    
    int fd_c2h = open("/dev/xdma0_c2h_0", O_RDWR);
    if (fd_c2h == -1) {
        perror("open /dev/xdma0_c2h_0");
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

    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, &features_length);
    coppy_features_to_matrix(features, raw_features, read_samples);
    load_model(tree_data, "../trained_models/diabetes.model");

    read_status(map_base, &status);

    send_features_ping_pong(fd_h2c, raw_features, 256, PONG);
    send_trees(fd_h2c, tree_data);

    set_burst_len(map_base, 256);
    set_ping_pong(map_base, PONG);
    start_prediction(map_base);
    read_status(map_base, &status);
    wait_done(map_base);

    read_prediction_ping_pong(fd_c2h, inference, 256, PONG);

    for ( i = 0; i < 256; i++){
        if (features[i].prediction == (inference[i] > 0))
            correct++;
    }

    close(fd_h2c);
    
    return 0;
}