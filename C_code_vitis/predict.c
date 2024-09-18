#include "predict.h"

void predict(uint64_t tree[N_TREES][N_NODE_AND_LEAFS],
            float bram_features[MAX_BURST_FEATURES][N_FEATURE], 
            int32_t prediction[MAX_BURST_FEATURES], uint8_t *features_burst_length){

    int32_t leaf_value;
    float local_features[N_FEATURE];

	#pragma HLS TOP name=predict
	#pragma HLS INTERFACE mode=s_axilite port=prediction bundle=prediction
	#pragma HLS INTERFACE mode=s_axilite port=bram_features bundle=bram_features
	#pragma HLS INTERFACE mode=s_axilite port=tree bundle=tree
	#pragma HLS INTERFACE s_axilite port=return bundle=control
	#pragma HLS ARRAY_PARTITION dim=1 factor=N_TREES type=block variable=tree
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=local_features



    burst_loop:for (int j = 0; j < *features_burst_length; j++){
	#pragma HLS loop_tripcount min=1 max=MAX_BURST_FEATURES

        coppy_loop:for (int i = 0; i < N_FEATURE; i++){
            local_features[i] = bram_features[j][i];
        }
        
	    int32_t sum = 0;

        trees_loop:for (int t = 0; t < N_TREES; t++){
        #pragma HLS UNROLL factor=N_TREES

            uint8_t node_index = 0;
            uint8_t node_right;
            uint8_t node_left;
            uint8_t feature_index;
            float threshold;
            tree_data tree_data;

            while(1){
            #pragma HLS loop_tripcount min=1 max=N_NODE_AND_LEAFS
                tree_data.compact_data = tree[t][node_index];
                feature_index = tree_data.tree_camps.feature_index;
                threshold = tree_data.tree_camps.float_int_union.f;
                node_left = node_index + 1;
                node_right = tree_data.tree_camps.next_node_right_index;

                node_index = *(int32_t*)&local_features[feature_index] < *(int32_t*)&threshold ? 
                                        node_left : node_right;


                if (!(tree_data.tree_camps.leaf_or_node & 0x01))
                    break;
            }

            leaf_value = tree_data.tree_camps.float_int_union.i;
            sum += leaf_value;
        }
        prediction[j] = sum;
    }

}
