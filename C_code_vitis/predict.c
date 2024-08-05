#include "predict.h"

void predict(tree_data tree[N_TREES][N_NODE_AND_LEAFS],
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
	#pragma HLS UNROLL factor=N_TREES

        uint8_t node_index = 0;
        uint8_t node_right;
        uint8_t node_left;
        uint8_t feature_index;
        float threshold;

        predict_label0:while (1) {
            #pragma HLS loop_tripcount min=1 max=N_NODE_AND_LEAFS
            feature_index = tree[t][node_index].tree_camps.feature_index;
            threshold = tree[t][node_index].tree_camps.float_int_union.f;
            node_left = node_index + 1;
            node_right = tree[t][node_index].tree_camps.next_node_right_index;

            node_index = *(int32_t*)&features[feature_index] < *(int32_t*)&threshold ? 
                                node_left : node_right;


           if (!tree[t][node_index].tree_camps.leaf_or_node)
                break;
        }

        leaf_value = tree[t][node_index].tree_camps.float_int_union.i;
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
