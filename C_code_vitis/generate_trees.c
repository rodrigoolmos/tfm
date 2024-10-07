#include "generate_trees.h"

uint8_t right_index[255] =  {128, 65, 34, 19, 12, 9, 8, 0, 0, 11, 0, 0, 16, 15, 0, 0, 18, 0, 0, 27, 24, 23, 0, 0, 26, 0, 0, 31, 30, 0, 0, 33, 0, 0, 50, 43, 40, 39, 0, 0, 42, 0, 0, 47, 46, 0, 0, 49, 0, 0, 58, 55, 54, 0, 0, 57, 0, 0, 62, 61, 0, 0, 64, 0, 0, 97, 82, 75, 72, 71, 0, 0, 74, 0, 0, 79, 78, 0, 0, 81, 0, 0, 90, 87, 86, 0, 0, 89, 0, 0, 94, 93, 0, 0, 96, 0, 0, 113, 106, 103, 102, 0, 0, 105, 0, 0, 110, 109, 0, 0, 112, 0, 0, 121, 118, 117, 0, 0, 120, 0, 0, 125, 124, 0, 0, 127, 0, 0, 192, 161, 146, 139, 136, 135, 0, 0, 138, 0, 0, 143, 142, 0, 0, 145, 0, 0, 154, 151, 150, 0, 0, 153, 0, 0, 158, 157, 0, 0, 160, 0, 0, 177, 170, 167, 166, 0, 0, 169, 0, 0, 174, 173, 0, 0, 176, 0, 0, 185, 182, 181, 0, 0, 184, 0, 0, 189, 188, 0, 0, 191, 0, 0, 224, 209, 202, 199, 198, 0, 0, 201, 0, 0, 206, 205, 0, 0, 208, 0, 0, 217, 214, 213, 0, 0, 216, 0, 0, 221, 220, 0, 0, 223, 0, 0, 240, 233, 230, 229, 0, 0, 232, 0, 0, 237, 236, 0, 0, 239, 0, 0, 248, 245, 244, 0, 0, 247, 0, 0, 252, 251, 0, 0, 254, 0, 0};


float generate_threshold() {
    float random_f = (float)rand() / (float)RAND_MAX;
    random_f *= 9999.999999999999999;
    if (rand() % 2 == 0) {
        random_f *= -1;
    }

    return random_f;
}

uint8_t generate_leaf_node(uint8_t prob__leaf_node) {
    uint8_t random_8 = (uint8_t)rand() % 100;

    return random_8 > prob__leaf_node;
}

uint8_t generate_feture_index(uint8_t feature_length) {
    uint8_t random_8 = (uint8_t)rand() % feature_length;

    return random_8;
}

void generate_rando_trees(tree_data trees[N_TREES][N_NODE_AND_LEAFS], 
                    uint8_t n_features, uint8_t n_trees){

    srand(clock());

    for (uint32_t tree_i = 0; tree_i < n_trees && tree_i < N_TREES; tree_i++){
        for (uint32_t node_i = 0; node_i < N_NODE_AND_LEAFS - 1; node_i++){
            trees[tree_i][node_i].tree_camps.feature_index = generate_feture_index(n_features);
            trees[tree_i][node_i].tree_camps.float_int_union.f = generate_threshold();
            trees[tree_i][node_i].tree_camps.leaf_or_node = 
                   (right_index[node_i] == 0) ? 0x00 : generate_leaf_node(30);
            trees[tree_i][node_i].tree_camps.next_node_right_index = right_index[node_i];
        }
    }
}

void mutate_trees(tree_data input_tree[N_TREES][N_NODE_AND_LEAFS], 
                 tree_data output_tree[N_TREES][N_NODE_AND_LEAFS],
                 uint8_t n_features, float mutation_rate, 
                 uint8_t n_trees){

    memcpy(output_tree, input_tree, sizeof(tree_data) * N_TREES * N_NODE_AND_LEAFS);

    srand(clock());
    for (uint32_t tree_i = 0; tree_i < n_trees && tree_i < N_TREES; tree_i++){
        for (uint32_t node_i = 0; node_i < N_NODE_AND_LEAFS - 1; node_i++){

            if ((float)rand() / (float)RAND_MAX < mutation_rate) {
                output_tree[tree_i][node_i].tree_camps.feature_index = generate_feture_index(n_features);
            }

            if ((float)rand() / (float)RAND_MAX < mutation_rate) {
                output_tree[tree_i][node_i].tree_camps.float_int_union.f = generate_threshold();
            }

            if ((float)rand() / (float)RAND_MAX < mutation_rate) {
                output_tree[tree_i][node_i].tree_camps.leaf_or_node = 
                    (right_index[node_i] == 0) ? 0x00 : generate_leaf_node(30);
            }

            output_tree[tree_i][node_i].tree_camps.next_node_right_index = right_index[node_i];
        }
    }
}