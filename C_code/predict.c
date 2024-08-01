#include "predict.h"

void predict(float node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
            uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS],
            uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS],
            float features[N_FEATURE], uint8_t *prediction){

	#pragma HLS INTERFACE mode=s_axilite port=prediction
	#pragma HLS INTERFACE mode=bram port=features
	#pragma HLS INTERFACE mode=bram port=compact_data
	#pragma HLS INTERFACE mode=bram port=node_leaf_value
	#pragma HLS INTERFACE mode=bram port=next_node_right_index
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=next_node_right_index
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=compact_data
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=node_leaf_value
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=features

	int32_t sum = 0;
    int32_t leaf_value;

    trees_loop:for (int t = 0; t < N_TREES; t++) {
	#pragma HLS UNROLL

        uint8_t node_index = 0;
        uint8_t node_right;
        uint8_t node_left;
        uint8_t feature_index;
        float threshold;
        uint8_t done = 0;

        while (1) {
            #pragma HLS loop_tripcount min=1 max=100
            feature_index = compact_data[t][node_index] & 0x7F;
            threshold = node_leaf_value[t][node_index];
            node_left = node_index + 1;
            node_right = next_node_right_index[t][node_index];

            node_index = (features[feature_index] <= threshold) ? node_left : node_right;

            done = (compact_data[t][node_index] & 0x80) == 0 ? 1 : 0;

           if (done)
                break;
        }

        leaf_value = *(int*)&node_leaf_value[t][node_index];
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
