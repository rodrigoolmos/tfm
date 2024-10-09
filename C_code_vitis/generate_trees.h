#include "predict.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <limits.h>



void generate_rando_trees(tree_data trees[N_TREES][N_NODE_AND_LEAFS], 
                    uint8_t n_features, uint8_t n_trees, float max_features[N_FEATURE], float min_features[N_FEATURE]);


void mutate_trees(tree_data input_tree[N_TREES][N_NODE_AND_LEAFS], 
                 tree_data output_tree[N_TREES][N_NODE_AND_LEAFS],
                 uint8_t n_features, float mutation_rate, 
                 uint8_t n_trees, float max_features[N_FEATURE], float min_features[N_FEATURE]);

void reproducee_trees(tree_data mother[N_TREES][N_NODE_AND_LEAFS],
                        tree_data father[N_TREES][N_NODE_AND_LEAFS],
                        tree_data son[N_TREES][N_NODE_AND_LEAFS],
                        uint8_t n_trees);