#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "./train/train.h"


#define N_ITE 200

int main() {

    uint32_t features_length, read_samples, i;
    uint32_t n_trees_used = 16;
    float accuracy = 0;

    tree_data tree_data[N_TREES][N_NODE_AND_LEAFS] = {0};
    struct feature features[MAX_TEST_SAMPLES] = {0};
    uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE] = {0};
    int32_t inference[MAX_TEST_SAMPLES] = {0};

    int fd_h2c = open("/dev/xdma0_h2c_0", O_RDWR);
    int fd_user = open("/dev/xdma0_user", O_RDWR);
    int fd_c2h = open("/dev/xdma0_c2h_0", O_RDWR);
    if (fd_h2c == -1){
        perror("open /dev/xdma0_h2c_0");
        return EXIT_FAILURE;
    }    
    if (fd_c2h == -1){
        perror("open /dev/xdma0_c2h_0");
        return EXIT_FAILURE;
    }
    if (fd_user == -1){
        perror("open /dev/xdma0_user");
        return EXIT_FAILURE;
    }

    train_model(fd_h2c,fd_c2h, fd_user, "../datasets/kaggle/Heart_Attack.csv", tree_data);


    
    close(fd_h2c);
    close(fd_c2h);
    
    return 0;
}