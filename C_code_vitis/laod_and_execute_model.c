#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "predict.h"

#define MAX_LINE_LENGTH 1024
#define MAX_COLUMNS 10

#define MAX_TEST_SAMPLES 3000

#define N_ITE 400


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

void evaluate_model(tree_data tree[N_TREES][N_NODE_AND_LEAFS], 
                    struct feature *features, int read_samples, 
                    float* time_used){

    int accuracy = 0;
    int32_t prediction;
    clock_t start_time, end_time;
    start_time = clock();

    for (size_t i = 0; i < read_samples; i++){
        predict(tree, features[i].features, &prediction);
        if (features[i].prediction == (prediction > 0))
            accuracy++;
    }

    printf("Accuracy %f\n", 1.0 * accuracy / read_samples);
    end_time = clock();
    *time_used = ((double)(end_time - start_time) / CLOCKS_PER_SEC)/read_samples;
    printf("Tiempo de ejecucion por feature: %f segundos\n", *time_used);
}

int main() {
    float prediction;
    float time_used = 0;
    float time_model_1 = 0;
    float time_model_2 = 0;
    float time_model_3 = 0;
    float time_model_4 = 0;
    float time_model_5 = 0;


    struct feature features[MAX_TEST_SAMPLES];
    int read_samples, i;
    tree_data tree_data[N_TREES][N_NODE_AND_LEAFS];
    for (i = 0; i < N_ITE; i++){
        printf("Executing SW\n");
        read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features);
        load_model(tree_data, "../trained_models/diabetes_1024.model");
        evaluate_model(tree_data, features, read_samples, &time_used);
        time_model_1 += time_used;
        read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features);
        load_model(tree_data, "../trained_models/heart_attack_1024.model");
        evaluate_model(tree_data, features, read_samples, &time_used);
        time_model_2 += time_used;
        read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features);
        load_model(tree_data, "../trained_models/lung_cancer_1024.model"); 
        evaluate_model(tree_data, features, read_samples, &time_used);
        time_model_3 += time_used;
        read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features);
        load_model(tree_data, "../trained_models/anemia_1024.model");
        evaluate_model(tree_data, features, read_samples, &time_used);
        time_model_4 += time_used;
        read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features);
        load_model(tree_data, "../trained_models/alzheimers_1024.model");
        evaluate_model(tree_data, features, read_samples, &time_used);
        time_model_5 += time_used;
    }
    
    printf("AVERAGE TIME CPU MODEL 1 %f\n", time_model_1 / N_ITE);
    printf("AVERAGE TIME CPU MODEL 2 %f\n", time_model_2 / N_ITE);
    printf("AVERAGE TIME CPU MODEL 3 %f\n", time_model_3 / N_ITE);
    printf("AVERAGE TIME CPU MODEL 4 %f\n", time_model_4 / N_ITE);
    printf("AVERAGE TIME CPU MODEL 5 %f\n", time_model_5 / N_ITE);
    return 0;
}