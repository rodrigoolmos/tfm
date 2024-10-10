#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "trees_managment.h"

#define TRAIN
//#define EVALUATE


#define MAX_LINE_LENGTH 1024
#define MAX_COLUMNS 10

#define MAX_TEST_SAMPLES 3000

struct feature {
    float features[N_FEATURE];
    uint8_t prediction;
};

struct dataset {
    struct feature *data;
    int num_rows;
    int num_cols;
};

void load_model(
            tree_data tree_data[N_TREES][N_NODE_AND_LEAFS],
            const char *filename) {

    char magic_number[5] = {0};
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening the file\n");
        return;
    }

    fread(magic_number, 5, 1, file);

    if (!memcmp(magic_number, "model", 5)){
        for (int t = 0; t < N_TREES; t++) {
            for (int n = 0; n < N_NODE_AND_LEAFS; n++) {
                fread(&tree_data[t][n], sizeof(uint64_t), 1, file);
            }
        }
    }else{
        perror("Unknown file type");
    }
    


    fclose(file);
}

int read_n_features(const char *csv_file, int n, struct feature *features) {
    FILE *file = fopen(csv_file, "r");
    char line[MAX_LINE_LENGTH];
    int features_read = 0;
    int i;

    if (!file) {
        perror("Failed to open the file");
        return -1;
    }

    while (fgets(line, MAX_LINE_LENGTH, file) && features_read < n) {
        float temp[N_FEATURE + 1];
        char *token = strtok(line, ",");
        int index = 0;

        while (token != NULL && index < N_FEATURE + 1) {
            temp[index] = atof(token);
            token = strtok(NULL, ",");
            index++;
        }

        for (i = 0; i < index - 1; i++) {
            features[features_read].features[i] = temp[i];
        }
        features[features_read].prediction = (uint8_t) temp[index - 1];

        features_read++;
    }

    fclose(file);
    return features_read;
}

void execute_model(tree_data tree[N_TREES][N_NODE_AND_LEAFS], 
                    struct feature *features, int read_samples, 
                    float *accuracy, uint8_t sow_log){

    int correct = 0;
    int32_t prediction;
    int32_t burst_size = 1;
    float features_burst[N_FEATURE];

    int ceil_div = (read_samples + MAX_BURST_FEATURES - 1) / MAX_BURST_FEATURES;

    for (int i = 0; i < read_samples; i++){
        memcpy(features_burst, features[i].features, sizeof(float) * N_FEATURE);  
        predict(tree, NULL, features_burst, NULL, &prediction, &burst_size, 0);
        if (features[i].prediction == (prediction > 0))
            correct++;
    }

    *accuracy = (float) correct / read_samples;
    
    if (sow_log)
        printf("Accuracy %f evaluates samples %i correct ones %i\n",
                        1.0 * (*accuracy), read_samples, correct);
    
}



void evaluate_model(tree_data tree[N_TREES][N_NODE_AND_LEAFS], 
                    struct feature *features, int read_samples){

    int accuracy = 0;
    int evaluated = 0;
    int32_t prediction[MAX_BURST_FEATURES];
    float features_burst[MAX_BURST_FEATURES][N_FEATURE];
    int32_t burst_size = MAX_BURST_FEATURES;
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    int ceil_div = (read_samples + MAX_BURST_FEATURES - 1) / MAX_BURST_FEATURES;

    // test pong
    for (int i = 0; i < ceil_div; i++){
        if (i == ceil_div -1){
            if (0 != read_samples % MAX_BURST_FEATURES){
                burst_size = read_samples % MAX_BURST_FEATURES;
            }
        }

        for (int j = 0; j < burst_size; j++){
                memcpy(features_burst[j], features[i * MAX_BURST_FEATURES + j].features, sizeof(float) *N_FEATURE);
        }
        
        predict(tree, NULL, features_burst, NULL, prediction, &burst_size, 0);

        for (int j = 0; j < burst_size; j++){
            if (features[i * MAX_BURST_FEATURES + j].prediction == (prediction[j] > 0)){
                accuracy++;
            }
            evaluated++;
        }
    }
    printf("PONG!!!!!!\n");
    printf("Accuracy %f evaluates samples %i of %i\n", 1.0 * accuracy / read_samples, evaluated, read_samples);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    //printf("Tiempo de ejecucion por feature: %f segundos\n", cpu_time_used / read_samples);
    
    
    // test ping
    accuracy = 0;
    evaluated = 0;
    burst_size = MAX_BURST_FEATURES;
    for (int i = 0; i < ceil_div; i++){
        if (i == ceil_div -1){
            if (0 != read_samples % MAX_BURST_FEATURES){
                burst_size = read_samples % MAX_BURST_FEATURES;
            }
        }

        for (int j = 0; j < burst_size; j++){
                memcpy(features_burst[j], features[i * MAX_BURST_FEATURES + j].features, sizeof(float) *N_FEATURE);
        }
        
        predict(tree, features_burst, NULL, prediction, NULL, &burst_size, 1);

        for (int j = 0; j < burst_size; j++){
            if (features[i * MAX_BURST_FEATURES + j].prediction == (prediction[j] > 0)){
                accuracy++;
            }
            evaluated++;
        }
    }
    printf("PING!!!!!!\n");
    printf("Accuracy %f evaluates samples %i of %i\n", 1.0 * accuracy / read_samples, evaluated, read_samples);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    //printf("Tiempo de ejecucion por feature: %f segundos\n\n\n", cpu_time_used / read_samples);
    
}

void swap(float *a, float *b) {
    float temp = *a;
    *a = *b;
    *b = temp;
}

void swap_trees(tree_data trees1[N_TREES][N_NODE_AND_LEAFS], 
                tree_data trees2[N_TREES][N_NODE_AND_LEAFS]) {
    tree_data temp_trees[N_TREES][N_NODE_AND_LEAFS];
    memcpy(temp_trees, trees1, sizeof(tree_data) * N_TREES * N_NODE_AND_LEAFS);
    memcpy(trees1, trees2, sizeof(tree_data) * N_TREES * N_NODE_AND_LEAFS);
    memcpy(trees2, temp_trees, sizeof(tree_data) * N_TREES * N_NODE_AND_LEAFS);
}

int partition(float population_accuracy[POPULATION], 
              tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS], 
              int low, int high) {
    float pivot = population_accuracy[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (population_accuracy[j] > pivot) {
            i++;
            swap(&population_accuracy[i], &population_accuracy[j]);
            swap_trees(trees_population[i], trees_population[j]);
        }
    }
    swap(&population_accuracy[i + 1], &population_accuracy[high]);
    swap_trees(trees_population[i + 1], trees_population[high]);
    return i + 1;
}

void quicksort(float population_accuracy[POPULATION], 
               tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS], 
               int low, int high) {
    if (low < high) {
        int pi = partition(population_accuracy, trees_population, low, high);

        quicksort(population_accuracy, trees_population, low, pi - 1);
        quicksort(population_accuracy, trees_population, pi + 1, high);
    }
}

void reorganize_population(float population_accuracy[POPULATION], 
                    tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS]) {
    quicksort(population_accuracy, trees_population, 0, POPULATION - 1);
}

void find_max_min_features(struct feature features[MAX_TEST_SAMPLES],
                                float max_features[N_FEATURE], float min_features[N_FEATURE]) {

    for (int j = 0; j < N_FEATURE; j++) {
        max_features[j] = features[0].features[j];
        min_features[j] = features[0].features[j];
    }

    for (int i = 1; i < MAX_TEST_SAMPLES; i++) {
        for (int j = 0; j < N_FEATURE; j++) {
            if (features[i].features[j] > max_features[j]) {
                max_features[j] = features[i].features[j];
            }
            if (features[i].features[j] < min_features[j]) {
                min_features[j] = features[i].features[j];
            }
        }
    }
}

int main() {
    float population_accuracy[POPULATION] = {0};
    float max_features[N_FEATURE];
    float min_features[N_FEATURE];

    struct feature features[MAX_TEST_SAMPLES];
    int read_samples;
    tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS];
    tree_data trees_test[N_TREES][N_NODE_AND_LEAFS];
    srand(clock());

#ifdef EVALUATE
    printf("Executing SW\n");
    printf("Executing diabetes.csv\n");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features);
    load_model(trees_test, "../trained_models/diabetes.model");
    evaluate_model(trees_test, features, read_samples);
    
    printf("Executing Heart_Attack.csv\n");
    read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features);
    load_model(trees_test, "../trained_models/heart_attack.model");
    evaluate_model(trees_test, features, read_samples);

    printf("Executing Lung_Cancer_processed_dataset.csv\n");
    read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features);
    load_model(trees_test, "../trained_models/lung_cancer.model");
    evaluate_model(trees_test, features, read_samples);

    printf("Executing anemia_processed_dataset.csv\n");
    read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features);
    load_model(trees_test, "../trained_models/anemia.model");
    evaluate_model(trees_test, features, read_samples);

    printf("Executing alzheimers_processed_dataset.csv\n");
    read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features);
    load_model(trees_test, "../trained_models/alzheimers.model");
    evaluate_model(trees_test, features, read_samples);
#endif

#ifdef TRAIN
    printf("Training model diabetes.csv\n");
    read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features);

    find_max_min_features(features, max_features, min_features);

    for (uint32_t p = 0; p < POPULATION; p++)
        generate_rando_trees(trees_population[p], 8, N_TREES, max_features, min_features);

    while(1){
        clock_t start1 = clock();
        for (uint32_t p = 0; p < POPULATION; p++)
            execute_model(trees_population[p], features, read_samples - read_samples/2, &population_accuracy[p], 0);
        clock_t start2 = clock();

        reorganize_population(population_accuracy, trees_population);
        /////////////////////////////// tests ///////////////////////////////
        for (int32_t p = POPULATION - 1; p >= 0; p--)
            printf("Popullation accuracy %i, %f\n", p, population_accuracy[p]);
        // evaluation features from out the training dataset
        evaluate_model(trees_population[0], &features[read_samples - read_samples/2], read_samples/2);
        /////////////////////////////////////////////////////////////////////
        if(population_accuracy[0] >= 0.95)
            break;

        for (uint32_t p = POPULATION/2; p < POPULATION; p++){
            int index_elite = rand() % (2*POPULATION/3);
            mutate_trees(trees_population[index_elite], trees_population[p], 
                                8, 1 - population_accuracy[p], N_TREES,
                                    max_features, min_features);
        }

        crossover(trees_population);

        clock_t end = clock();
        printf("Execution time inference %f, rest %f\n", ((float)start2-start1)/CLOCKS_PER_SEC, ((float)end-start2)/CLOCKS_PER_SEC);
    }

    evaluate_model(trees_population[0], &features[read_samples - read_samples/2], read_samples/2);
#endif
    return 0;
}