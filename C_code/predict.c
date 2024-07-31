#include "predict.h"

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
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
