#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "./burst_drivers/burst.h"

int main() {

    int read_samples, i, correct;
    uint32_t status;
    uint32_t features_length;

    
    int fd;
    void *map_base;

    tree_data tree_data[N_TREES][N_NODE_AND_LEAFS] = {0};
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

    load_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
    load_model(tree_data, "../trained_models/diabetes.model");
    evaluate_model(fd_h2c, fd_c2h,tree_data, map_base, features, raw_features, inference, read_samples);

    load_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
    load_model(tree_data, "../trained_models/heart_attack.model");
    evaluate_model(fd_h2c, fd_c2h,tree_data, map_base, features, raw_features, inference, read_samples);

    load_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
    load_model(tree_data, "../trained_models/lung_cancer.model");
    evaluate_model(fd_h2c, fd_c2h,tree_data, map_base, features, raw_features, inference, read_samples);

    load_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
    load_model(tree_data, "../trained_models/anemia.model");
    evaluate_model(fd_h2c, fd_c2h,tree_data, map_base, features, raw_features, inference, read_samples);

    load_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
    load_model(tree_data, "../trained_models/alzheimers.model");
    evaluate_model(fd_h2c, fd_c2h,tree_data, map_base, features, raw_features, inference, read_samples);

    close(fd_h2c);
    close(fd_c2h);

    
    return 0;
}