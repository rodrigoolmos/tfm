#include "predict.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>


void generate_rando_trees(tree_data trees[N_TREES][N_NODE_AND_LEAFS], 
                uint8_t n_features, uint8_t n_trees);