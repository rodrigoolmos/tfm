#include <stdint.h>

#define N_NODE_AND_LEAFS 256  // Adjust according to the maximum number of nodes and leaves in your trees
#define N_TREES 100           // Adjust according to the number of trees in your model
#define N_FEATURE 100    // Adjust according to the number of features in your model


struct tree {
    float node_leaf_value[N_NODE_AND_LEAFS];            // value of the leaf result or the node to compare
    uint8_t feature_index[N_NODE_AND_LEAFS];            // feature index value for comparison
    uint8_t next_node_right_index[N_NODE_AND_LEAFS];    // model index of the next node or leaf if the comparison is >
    uint8_t leaf_or_node[N_NODE_AND_LEAFS];             // information about whether it is a node or a leaf
};

void predict(struct tree trees[N_TREES], float features[N_FEATURE], uint8_t *prediction);