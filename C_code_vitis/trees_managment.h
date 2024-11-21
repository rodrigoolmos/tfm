#include "predict.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>
#include <omp.h>

#define POPULATION 1024
#define MEMORY_ACU_SIZE 10


void generate_rando_trees(tree_data trees[N_TREES][N_NODE_AND_LEAFS], 
                    uint8_t n_features, uint8_t n_trees, float max_features[N_FEATURE], float min_features[N_FEATURE]);

void mutate_population(tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS],
                        float population_accuracy[POPULATION], float max_features[N_FEATURE],
                        float min_features[N_FEATURE], uint8_t n_features, float noise_factor);

void crossover(tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS]);

void reorganize_population(float population_accuracy[POPULATION], 
                    tree_data trees_population[POPULATION][N_TREES][N_NODE_AND_LEAFS]);