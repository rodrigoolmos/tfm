#include "predict.h"

void predict(uint64_t tree[N_TREES][N_NODE_AND_LEAFS],
            float features[N_FEATURE], uint8_t *prediction){

	#pragma HLS TOP name=predict

	#pragma HLS INTERFACE mode=s_axilite port=prediction bundle=prediction
	#pragma HLS INTERFACE mode=s_axilite port=features bundle=features
	#pragma HLS INTERFACE mode=s_axilite port=tree bundle=tree
	#pragma HLS INTERFACE s_axilite port=return bundle=control

	#pragma HLS ARRAY_PARTITION dim=1 factor=N_TREES/2 type=block variable=tree
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
        tree_data tree_data;

        while(1){
            tree_data.compact_data = tree[t][node_index];
            #pragma HLS loop_tripcount min=1 max=N_NODE_AND_LEAFS
            feature_index = tree_data.tree_camps.feature_index;
            threshold = tree_data.tree_camps.float_int_union.f;
            node_left = node_index + 1;
            node_right = tree_data.tree_camps.next_node_right_index;

            node_index = *(int32_t*)&features[feature_index] < *(int32_t*)&threshold ? 
                                    node_left : node_right;


            if (tree_data.tree_camps.leaf_or_node & 0x01)
            	break;
        }

        leaf_value = tree_data.tree_camps.float_int_union.i;
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
