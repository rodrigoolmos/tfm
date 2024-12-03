#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <omp.h>
#include "../burst_drivers/burst.h"

#define POPULATION 1024             // population train with
#define MEMORY_ACU_SIZE 10          // size to be remembered between generations for Mutation_factor
#define BAGGING_FACTOR 64            
#define AUGMENT_FACTOR 1            // xtimes de actual size of features
#define DSS_GEN_VALUE 10            // each gen times the ds gets suffled
#define PRECISION_COND_EXIT 1       // accurazy to exit
#define ITERATIONS_COND_EXIT 100    // num generation to exit
#define N_TREES_BAGGING N_TREES / BAGGING_FACTOR

void generate_rando_trees(tree_data trees[N_TREES_BAGGING][N_NODE_AND_LEAFS], 
                    uint8_t n_features, uint16_t n_trees, float max_features[N_FEATURE], float min_features[N_FEATURE]);

void mutate_population(tree_data trees_population[POPULATION][N_TREES_BAGGING][N_NODE_AND_LEAFS],
                        float population_accuracy[POPULATION], float max_features[N_FEATURE],
                        float min_features[N_FEATURE], uint8_t n_features, float noise_factor);

void crossover(tree_data trees_population[POPULATION][N_TREES_BAGGING][N_NODE_AND_LEAFS]);

void reorganize_population(float population_accuracy[POPULATION], 
                    tree_data trees_population[POPULATION][N_TREES_BAGGING][N_NODE_AND_LEAFS]);

int augment_features(const struct feature *original_features, int n_features, int n_col,
                     float max_features[N_FEATURE], float min_features[N_FEATURE],
                     struct feature *augmented_features, int max_augmented_features, 
                     int augmentation_factor);

void train_model(int fd_h2c, int fd_c2h, int fd_user, char *csv_path, 
                tree_data trained_model[N_TREES][N_NODE_AND_LEAFS]);