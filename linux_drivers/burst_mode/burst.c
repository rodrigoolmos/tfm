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

int main() {

    int read_samples;
    uint32_t features_length;
    
    int fd;
    void *map_base;

    tree_data tree_data[N_TREES][N_NODE_AND_LEAFS];
    struct feature features[MAX_TEST_SAMPLES] = {0};

    uint32_t test_send[256] = {0};
    uint32_t test_recive[256] = {0};

    for (size_t i = 0; i < 256; i++){
        test_send[i] = i;
    }
    

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
    load_model(tree_data, "../trained_models/diabetes.model");

    write_to_burst(fd_h2c, FEATURES_PING_ADDR, test_send, 256 * sizeof(uint32_t));
    read_from_burst(fd_c2h, FEATURES_PING_ADDR, test_recive, 256 * sizeof(uint32_t));

    write_to_burst(fd_h2c, FEATURES_PONG_ADDR, test_send, 256 * sizeof(uint32_t));
    read_from_burst(fd_c2h, FEATURES_PONG_ADDR, test_recive, 256 * sizeof(uint32_t));

    write_to_burst(fd_h2c, PREDICTIONS_PING_ADDR, test_send, 256 * sizeof(uint32_t));
    read_from_burst(fd_c2h, PREDICTIONS_PING_ADDR, test_recive, 256 * sizeof(uint32_t));

    write_to_burst(fd_h2c, PREDICTIONS_PONG_ADDR, test_send, 256 * sizeof(uint32_t));
    read_from_burst(fd_c2h, PREDICTIONS_PONG_ADDR, test_recive, 256 * sizeof(uint32_t));

    write_to_burst(fd_h2c, TREES_ADDR, test_send, 256 * sizeof(uint32_t));
    read_from_burst(fd_c2h, TREES_ADDR, test_recive, 256 * sizeof(uint32_t));

    close(fd_h2c);
    
    return 0;
}