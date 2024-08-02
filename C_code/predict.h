#include <stdint.h>

#define N_NODE_AND_LEAFS 64  // Adjust according to the maximum number of nodes and leaves in your trees
#define N_TREES 10           // Adjust according to the number of trees in your model
#define N_FEATURE 50    // Adjust according to the number of features in your model

typedef union {
    float f;
    int32_t i;
} float_int_union;

void predict(float_int_union node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
            uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS],
            uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS],
            float features[N_FEATURE], uint8_t *prediction);
