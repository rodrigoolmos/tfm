#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "burst.h"

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

void send_features_ping_pong(int fd_h2c, uint32_t* features, uint32_t n_features, uint32_t ping_pong){
    
    if (ping_pong)
        write_burst(fd_h2c, FEATURES_PING_ADDR, features, n_features * N_FEATURE * sizeof(uint32_t));
    else
        write_burst(fd_h2c, FEATURES_PONG_ADDR, features, n_features * N_FEATURE * sizeof(uint32_t));
}

void read_prediction_ping_pong(void *map_base, int fd_c2h, int32_t* predictions, uint32_t n_predictions, uint32_t ping_pong){
    
    if (ping_pong){
        wait_done(map_base);
        read_burst(fd_c2h, PREDICTIONS_PING_ADDR, predictions, n_predictions * sizeof(uint32_t));

    }
    else{
        wait_done(map_base);
        read_burst(fd_c2h, PREDICTIONS_PONG_ADDR, predictions, n_predictions * sizeof(uint32_t));
    }
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

void start_prediction_ping_pong(void *map_base, uint32_t ping_pong, uint32_t burst_len){
    
    if (ping_pong){
        set_burst_len(map_base, burst_len);
        set_ping_pong(map_base, PING);
        start_prediction(map_base);
    }else{
        set_burst_len(map_base, burst_len);
        set_ping_pong(map_base, PONG);
        start_prediction(map_base);
    }
    

}

void copy_features_to_matrix(struct feature* features, 
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], uint32_t features_length){


    for (uint32_t i = 0; i < features_length; i++)
        memcpy(raw_features[i], features[i].features, sizeof(uint32_t)*N_FEATURE);    

}

void burst_ping_pong_process(void *map_base, int fd_h2c, int fd_c2h, 
                    uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int32_t n_features_total, int32_t* inference){

    uint32_t ping_pong = PING;
    int32_t features_to_precess = 0;
    int32_t features_index = 0;


    if (n_features_total <= MAX_BURST){

        send_features_ping_pong(fd_h2c, raw_features[0], n_features_total, ping_pong);
        start_prediction_ping_pong(map_base, ping_pong, n_features_total);
        read_prediction_ping_pong(map_base, fd_c2h, inference, MAX_BURST, ping_pong);

    }else{

        send_features_ping_pong(fd_h2c, raw_features[0], MAX_BURST, ping_pong);
        start_prediction_ping_pong(map_base, ping_pong, MAX_BURST);
        features_index = features_index + MAX_BURST;
        features_to_precess = n_features_total > MAX_BURST ? MAX_BURST : n_features_total;

        while (n_features_total >= 0){
            n_features_total = n_features_total - MAX_BURST;
            if (n_features_total >= 0)
                send_features_ping_pong(fd_h2c, raw_features[features_index], features_to_precess, ~ping_pong);
            read_prediction_ping_pong(map_base, fd_c2h, &inference[features_index - features_to_precess], features_to_precess, ping_pong);
            if (n_features_total >= 0)
                start_prediction_ping_pong(map_base, ~ping_pong, features_to_precess);
            ping_pong = ~ping_pong;
            features_to_precess = n_features_total > MAX_BURST ? MAX_BURST : n_features_total;
            features_index = features_index + features_to_precess;
        }

    }

}

void evaluate_model(int fd_h2c, int fd_c2h, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS],
                    void *map_base, struct feature features[MAX_TEST_SAMPLES], uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE],
                    int32_t inference[MAX_TEST_SAMPLES], uint32_t read_samples){
    clock_t start_time, end_time;
    double cpu_time_used;
    int i, correct = 0;


    send_trees(fd_h2c, tree_data);

    start_time = clock();
    burst_ping_pong_process(map_base, fd_h2c, fd_c2h, raw_features, read_samples, inference);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;           

    for ( i = 0; i < read_samples; i++){
        if (features[i].prediction == (inference[i] > 0))
            correct++;
    }

    printf("Execution time per 100 features: %f seconds\n", (100 * cpu_time_used) / read_samples);
    printf("Accuracy %f\n", 1.0 * correct / read_samples);

}

void load_features(const char* filename, int max_test_samples, struct feature* features,
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int* features_length, int* read_samples) {

    *read_samples = read_n_features(filename, max_test_samples, features, features_length);
    copy_features_to_matrix(features, raw_features, *read_samples);
}