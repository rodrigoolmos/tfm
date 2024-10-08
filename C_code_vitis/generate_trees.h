#include "predict.h"
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>


void generate_rando_trees(tree_data trees[N_TREES][N_NODE_AND_LEAFS], 
                uint8_t n_features, uint8_t n_trees);


void mutate_trees(tree_data input_tree[N_TREES][N_NODE_AND_LEAFS], 
                 tree_data output_tree[N_TREES][N_NODE_AND_LEAFS],
                 uint8_t n_features, float mutation_rate, 
                 uint8_t n_trees);