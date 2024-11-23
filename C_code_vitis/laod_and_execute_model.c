#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "train.h"

#define MAX_LINE_LENGTH 1024

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

int read_n_features(const char *csv_file, int n, struct feature *features, int *n_col) {
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
        *n_col = 0;

        while (token != NULL && (*n_col) < N_FEATURE + 1) {
            temp[*n_col] = atof(token);
            token = strtok(NULL, ",");
            (*n_col)++;
        }

        for (i = 0; i < *n_col - 1; i++) {
            features[features_read].features[i] = temp[i];
        }
        features[features_read].prediction = (uint8_t) temp[*n_col - 1];

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
    uint32_t trees_score = 0;
    int32_t burst_size = 1;
    float features_burst[N_FEATURE];

    int ceil_div = (read_samples + MAX_BURST_FEATURES - 1) / MAX_BURST_FEATURES;

    for (int i = 0; i < read_samples; i++){
        memcpy(features_burst, features[i].features, sizeof(float) * N_FEATURE);  
        predict(tree, NULL, features_burst, NULL, &prediction, &burst_size, 0);
        if (features[i].prediction == (prediction > 0)){
            trees_score += abs(prediction);
            correct++;
        }
    }

    *accuracy = (float) correct / read_samples;
    *accuracy += ((0.01*trees_score) / (read_samples * N_TREES));
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
    //printf("PONG!!!!!!\n");
    printf("Accuracy real %f evaluates samples %i of %i\n", 1.0 * accuracy / read_samples, evaluated, read_samples);
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
    // printf("PING!!!!!!\n");
    // printf("Accuracy %f evaluates samples %i of %i\n", 1.0 * accuracy / read_samples, evaluated, read_samples);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    //printf("Tiempo de ejecucion por feature: %f segundos\n\n\n", cpu_time_used / read_samples);
    
}

void show_logs(float population_accuracy[POPULATION]){

        for (int32_t p = POPULATION/10; p >= 0; p--){
            printf("RANKING %i -> %f \t| RANKING %i -> %f \t| RANKING %i -> %f \t| RANKING %i -> %f| RANKING %i -> %f\n"
                            , p, population_accuracy[p]
                            , p + POPULATION/20, population_accuracy[p + POPULATION/20]
                            , p + POPULATION/10, population_accuracy[p + POPULATION/10]
                            , p + POPULATION/4 , population_accuracy[p + POPULATION/4]
                            , p + POPULATION/2 , population_accuracy[p + POPULATION/2]);
        }
}

int main() {
    float population_accuracy[POPULATION] = {0};
    float iteration_accuracy[MEMORY_ACU_SIZE] = {0};
    float mutation_factor = 0;
    float max_features[N_FEATURE];
    float min_features[N_FEATURE];
    
    struct feature features[MAX_TEST_SAMPLES];
    int read_samples;
    int generation_ite = 0;
    srand(clock());

    tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS];
    char *path ="/home/rodrigo/Documents/tfm/datasets/SoA/paper1/haberman.csv";

    printf("Training model %s\n", path);
    int n_features;
    read_samples = read_n_features(path, MAX_TEST_SAMPLES, features, &n_features);
    n_features--; // remove predictions

    shuffle(features, read_samples);

    find_max_min_features(features, max_features, min_features);

    for (uint32_t p = 0; p < POPULATION; p++)
        generate_rando_trees(trees_population[p], n_features, N_TREES, max_features, min_features);

    while(1){
        
        shuffle(features, read_samples* 80/100);

        clock_t t1 = clock();
        for (uint32_t p = 0; p < POPULATION; p++)
            execute_model(trees_population[p], features, read_samples * 70/100, &population_accuracy[p], 0);
        clock_t t2 = clock();

        reorganize_population(population_accuracy, trees_population);
        clock_t t3 = clock();

        /////////////////////////////// tests ///////////////////////////////

        show_logs(population_accuracy);

        // evaluation features from out the training dataset
        evaluate_model(trees_population[0], &features[read_samples * 80/100], read_samples * 20/100);
        /////////////////////////////////////////////////////////////////////

        if(population_accuracy[0] >= 1){
            break;
        }

        mutate_population(trees_population, population_accuracy, max_features, min_features, n_features, mutation_factor);

        clock_t t4 = clock();
        crossover(trees_population);
        clock_t t5 = clock();

        generation_ite ++;
        mutation_factor = 0;
        iteration_accuracy[generation_ite % MEMORY_ACU_SIZE] = population_accuracy[0];
        for (int accuracy_i = 0; accuracy_i < MEMORY_ACU_SIZE; accuracy_i++){
            if(iteration_accuracy[generation_ite % MEMORY_ACU_SIZE] == iteration_accuracy[accuracy_i]){
                if ((generation_ite % MEMORY_ACU_SIZE) != accuracy_i){
                    mutation_factor += 0.03;
                }
            }
        }

        printf("Mutation_factor %f\n", mutation_factor);
        printf("Generation ite %i index ite %i\n", generation_ite, generation_ite % 10);
        printf("Execution time inference %f, reorganize_population %f,"
                                    "mutate_population %f, crossover %f \n\n\n", ((float)t2-t1)/CLOCKS_PER_SEC, 
                                    ((float)t3-t2)/CLOCKS_PER_SEC, ((float)t4-t3)/CLOCKS_PER_SEC, ((float)t5-t4)/CLOCKS_PER_SEC);
    }

    printf("Final evaluation !!!!\n\n");
    evaluate_model(trees_population[0], &features[read_samples * 80/100], read_samples * 20/100);
    return 0;

}