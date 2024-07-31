#include "predict.h"

<<<<<<< Updated upstream
void predict(uint8_t tree_leaf_node[N_TREES][N_NODE_AND_LEAFS], 
             uint8_t tree_right_indexs[N_TREES][N_NODE_AND_LEAFS],
             uint8_t tree_feture_indexs[N_TREES][N_NODE_AND_LEAFS],
             float tree_node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
             float features[N_FEATURE], uint8_t *prediction) {

    int32_t sum = 0;
    int32_t leaf_value;

    trees_loop:for (int t = 0; t < N_TREES; t++) {
        int node_index = 0;
        int feature_index;
        float threshold;
        int next_node_index;

        while (tree_leaf_node[t][node_index] == 1) {
            #pragma HLS loop_tripcount min=1 max=100
            feature_index = tree_feture_indexs[t][node_index];
            threshold = tree_node_leaf_value[t][node_index];

            next_node_index = (features[feature_index] <= threshold) ?
            				node_index + 1 : tree_right_indexs[t][node_index];

            node_index = next_node_index;
        }
        leaf_value = *(int*)&tree_node_leaf_value[t][node_index];
=======
void predict(float node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
            uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS],
            uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS],
            float features[N_FEATURE], uint8_t *prediction){

	int32_t sum = 0;
    int32_t leaf_value;

    trees_loop:for (int t = 0; t < N_TREES; t++) {

        uint8_t node_index = 0;
        float node_value;
        uint8_t next_node_index;
        uint8_t feature_index;

        while ((compact_data[t][node_index] & 0x80)) {

            node_value = node_leaf_value[t][node_index];
            feature_index = (compact_data[t][node_index] & 0x7F);

            next_node_index = ( node_value > features[feature_index]) ?
            				    node_index + 1 : next_node_right_index[t][node_index];

            node_index = next_node_index;
        }
        leaf_value = *(int*)&node_leaf_value[t][node_index];
>>>>>>> Stashed changes
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
