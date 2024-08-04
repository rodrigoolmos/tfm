#include "predict.h"

void predict(float_int_union node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
            uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS],
            uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS],
            float features[N_FEATURE], uint8_t *prediction){

	#pragma HLS TOP name=predict

	#pragma HLS INTERFACE mode=s_axilite port=prediction bundle=prediction
	#pragma HLS INTERFACE mode=s_axilite port=features bundle=features
	#pragma HLS INTERFACE mode=s_axilite port=next_node_right_index bundle=next_node_right_index
	#pragma HLS INTERFACE mode=s_axilite port=compact_data bundle=compact_data
	#pragma HLS INTERFACE mode=s_axilite port=node_leaf_value bundle=node_leaf_value
	#pragma HLS INTERFACE s_axilite port=return bundle=control

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

        while (1) {
            #pragma HLS loop_tripcount min=1 max=100
            feature_index = compact_data[t][node_index] & 0x7F;
            threshold = node_leaf_value[t][node_index].f;
            node_left = node_index + 1;
            node_right = next_node_right_index[t][node_index];

            node_index = *(int32_t*)&features[feature_index] < *(int32_t*)&threshold ? 
                                node_left : node_right;


           if (!(compact_data[t][node_index] & 0x80))
                break;
        }

        leaf_value = node_leaf_value[t][node_index].i;
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
