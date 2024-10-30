#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include "./burst_drivers/burst.h"

#define N_ITE 1000

int main() {

    uint32_t features_length, read_samples, i;
    float time_ex = 0;
    float time_ex1 = 0;
    float time_ex2 = 0;
    float time_ex3 = 0;
    float time_ex4 = 0;
    float time_ex5 = 0;



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

    for (i = 0; i < N_ITE; i++){

        load_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
        load_model(tree_data, "../trained_models/diabetes.model");
        evaluate_model(fd_h2c, fd_c2h,tree_data, fd_user, features, raw_features, inference, read_samples, &time_ex);
        time_ex1 += time_ex;
        ///////////////////////////////////   example only inference   //////////////////////////
        burst_ping_pong_process(fd_user, fd_h2c, fd_c2h, raw_features, read_samples, inference);
        /////////////////////////////////////////////////////////////////////////////////////////
        
        load_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
        load_model(tree_data, "../trained_models/heart_attack.model");
        evaluate_model(fd_h2c, fd_c2h,tree_data, fd_user, features, raw_features, inference, read_samples, &time_ex);
        time_ex2 += time_ex;


        load_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
        load_model(tree_data, "../trained_models/lung_cancer.model");
        evaluate_model(fd_h2c, fd_c2h,tree_data, fd_user, features, raw_features, inference, read_samples, &time_ex);
        time_ex3 += time_ex;

        load_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
        load_model(tree_data, "../trained_models/anemia.model");
        evaluate_model(fd_h2c, fd_c2h,tree_data, fd_user, features, raw_features, inference, read_samples, &time_ex);
        time_ex4 += time_ex;

        load_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, raw_features, &features_length, &read_samples);
        load_model(tree_data, "../trained_models/alzheimers.model");
        evaluate_model(fd_h2c, fd_c2h,tree_data, fd_user, features, raw_features, inference, read_samples, &time_ex);
        time_ex5 += time_ex;
    
    }

    printf("AVERAGE TIME EXECUTION MODEL 1 %f\n", time_ex1 / N_ITE);
    printf("AVERAGE TIME EXECUTION MODEL 2 %f\n", time_ex2 / N_ITE);
    printf("AVERAGE TIME EXECUTION MODEL 3 %f\n", time_ex3 / N_ITE);
    printf("AVERAGE TIME EXECUTION MODEL 4 %f\n", time_ex4 / N_ITE);
    printf("AVERAGE TIME EXECUTION MODEL 5 %f\n", time_ex5 / N_ITE);


    close(fd_h2c);
    close(fd_c2h);

    
    return 0;
}