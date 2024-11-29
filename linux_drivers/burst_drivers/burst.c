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

void wait_done(int fd_user){

    uint32_t data;

    while (1){
        pread(fd_user, &data, 4, CONTROL_ADDR);  
        if (data&0x2)
            break;
    }
}


void set_burst_len(int fd_user, uint32_t burst_len){
    if (pwrite(fd_user, &burst_len, 4, BURST_LENGTH_ADDR) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }

}

void set_ping_pong(int fd_user, uint32_t ping_pong){
    if (pwrite(fd_user, &ping_pong, 4, PING_PONG_ADDR) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
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

void send_features_ping_pong(int fd_h2c, uint32_t* features, uint32_t n_features, uint32_t ping_pong){
    
    if (ping_pong)
        write_burst(fd_h2c, FEATURES_PING_ADDR, features, n_features * N_FEATURE * sizeof(uint32_t));
    else
        write_burst(fd_h2c, FEATURES_PONG_ADDR, features, n_features * N_FEATURE * sizeof(uint32_t));
}

void read_prediction_ping_pong(int fd_user, int fd_c2h, int32_t* predictions, uint32_t n_predictions, uint32_t ping_pong){
    
    if (ping_pong){
        wait_done(fd_user);
        read_burst(fd_c2h, PREDICTIONS_PING_ADDR, predictions, n_predictions * sizeof(uint32_t));

    }
    else{
        wait_done(fd_user);
        read_burst(fd_c2h, PREDICTIONS_PONG_ADDR, predictions, n_predictions * sizeof(uint32_t));
    }
}

void load_trees_from_ram(int fd_user){
    
    uint32_t data = 0x01;

    if (pwrite(fd_user, &data, 4, LOAD_TREES_ADDR) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void set_trees_used(int fd_user, uint32_t *n_trees_used){
    
    if (pwrite(fd_user, n_trees_used, 4, TREES_USED_ADDR) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void send_trees(int fd_user, int fd_h2c, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS], uint32_t *n_trees_used){

    int i;
    uint64_t offset;

    *n_trees_used = *n_trees_used < N_TREES ? *n_trees_used : N_TREES;
    
    set_trees_used(fd_user, n_trees_used);
    load_trees_from_ram(fd_user);

    for (i = 0; i < *n_trees_used; i++){
        offset =  N_NODE_AND_LEAFS * sizeof(uint64_t) * i;
        write_burst(fd_h2c, TREES_ADDR + offset, tree_data[i], N_NODE_AND_LEAFS * sizeof(uint64_t));
    }

}

void read_status(int fd_user, uint32_t *data){

    if (pread(fd_user, data, 4, CONTROL_ADDR) == -1) {
        perror("pread");
        exit(EXIT_FAILURE);
    }

}

void start_prediction(int fd_user){
    
    uint32_t data = 0x01;

    if (pwrite(fd_user, &data, 4, CONTROL_ADDR) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void start_prediction_ping_pong(int fd_user, uint32_t ping_pong, uint32_t burst_len){
    
    if (ping_pong){
        set_burst_len(fd_user, burst_len);
        set_ping_pong(fd_user, PING);
        start_prediction(fd_user);
    }else{
        set_burst_len(fd_user, burst_len);
        set_ping_pong(fd_user, PONG);
        start_prediction(fd_user);
    }
    

}

void copy_features_to_matrix(struct feature* features, 
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], uint32_t features_length){


    for (uint32_t i = 0; i < features_length; i++)
        memcpy(raw_features[i], features[i].features, sizeof(uint32_t)*N_FEATURE);    

}

void burst_ping_pong_process(int fd_user, int fd_h2c, int fd_c2h, 
                    uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int32_t n_features_total, int32_t* inference){

    uint32_t ping_pong = PING;
    int32_t features_to_precess = 0;
    int32_t features_index = 0;


    if (n_features_total <= MAX_BURST){

        send_features_ping_pong(fd_h2c, raw_features[0], n_features_total, ping_pong);
        start_prediction_ping_pong(fd_user, ping_pong, n_features_total);
        read_prediction_ping_pong(fd_user, fd_c2h, inference, MAX_BURST, ping_pong);

    }else{

        send_features_ping_pong(fd_h2c, raw_features[0], MAX_BURST, ping_pong);
        start_prediction_ping_pong(fd_user, ping_pong, MAX_BURST);
        features_index = features_index + MAX_BURST;
        features_to_precess = n_features_total > MAX_BURST ? MAX_BURST : n_features_total;

        while (n_features_total >= 0){
            n_features_total = n_features_total - MAX_BURST;
            if (n_features_total >= 0)
                send_features_ping_pong(fd_h2c, raw_features[features_index], features_to_precess, ~ping_pong);
            read_prediction_ping_pong(fd_user, fd_c2h, &inference[features_index - features_to_precess], features_to_precess, ping_pong);
            if (n_features_total >= 0)
                start_prediction_ping_pong(fd_user, ~ping_pong, features_to_precess);
            ping_pong = ~ping_pong;
            features_to_precess = n_features_total > MAX_BURST ? MAX_BURST : n_features_total;
            features_index = features_index + features_to_precess;
        }

    }

}

void evaluate_model(int fd_h2c, int fd_c2h, tree_data tree_data[][N_NODE_AND_LEAFS],
                    int fd_user, struct feature features[MAX_TEST_SAMPLES], uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE],
                    uint32_t read_samples, float* accuracy, uint32_t *n_trees_used){
    clock_t start_time, end_time;
    double cpu_time_used;
    int i, correct = 0;
    int32_t inference[MAX_TEST_SAMPLES];

    send_trees(fd_user, fd_h2c, tree_data, n_trees_used);

    start_time = clock();
    burst_ping_pong_process(fd_user, fd_h2c, fd_c2h, raw_features, read_samples, inference);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;           

    for ( i = 0; i < read_samples; i++){
        if (features[i].prediction == (inference[i] > 0))
            correct++;
    }
    *accuracy = 1.0 * correct / read_samples;
}

void load_features(const char* filename, int max_test_samples, struct feature* features,
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int* features_length, int* read_samples) {

    *read_samples = read_n_features(filename, max_test_samples, features, features_length);
    copy_features_to_matrix(features, raw_features, *read_samples);
}