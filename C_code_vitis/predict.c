#include "predict.h"

void predict(uint64_t tree[N_TREES_IP][N_NODE_AND_LEAFS],
            float bram_features_ping[MAX_BURST_FEATURES][N_FEATURE],
            float bram_features_pong[MAX_BURST_FEATURES][N_FEATURE], 
            int32_t prediction_ping[MAX_BURST_FEATURES],
            int32_t prediction_pong[MAX_BURST_FEATURES], 
            int32_t *features_burst_length,
            int32_t ping_pong){

    int32_t leaf_value;
    static int8_t local_ping_pong;
    float local_features_ping[N_FEATURE];
    float local_features_pong[N_FEATURE];

	#pragma HLS TOP name=predict
	#pragma HLS INTERFACE mode=bram port=prediction_ping
	#pragma HLS INTERFACE mode=bram port=prediction_pong
	#pragma HLS INTERFACE mode=bram port=bram_features_ping
	#pragma HLS INTERFACE mode=bram port=bram_features_pong
	#pragma HLS INTERFACE mode=s_axilite port=tree bundle=tree
	#pragma HLS INTERFACE mode=s_axilite port=features_burst_length bundle=control
	#pragma HLS INTERFACE mode=s_axilite port=ping_pong bundle=control
	#pragma HLS INTERFACE s_axilite port=return bundle=control
	#pragma HLS ARRAY_PARTITION dim=1 factor=N_TREES_IP type=block variable=tree
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=local_features_ping
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=local_features_pong

    if (!local_ping_pong){
        coppy_loop_pong_init: for (int i = 0; i < N_FEATURE; i++){
            local_features_pong[i] = ping_pong & 0x01 ? bram_features_ping[0][i] : bram_features_pong[0][i];
        }
    }else{
        coppy_loop_ping_init: for (int i = 0; i < N_FEATURE; i++){
            local_features_ping[i] = ping_pong & 0x01 ? bram_features_ping[0][i] : bram_features_pong[0][i];
        }
    }
    
    burst_loop:for (int j = 0; j < *features_burst_length; j++){
	#pragma HLS loop_tripcount min=1 max=MAX_BURST_FEATURES

	    int32_t sum = 0;

        if (local_ping_pong){

            trees_loop_ping: for (int t = 0; t < N_TREES_IP && t < N_TREES; t++){
            #pragma HLS UNROLL factor=N_TREES_IP

                uint8_t node_index = 0;
                uint8_t node_right;
                uint8_t node_left;
                uint8_t feature_index;
                float threshold;
                tree_data tree_data;

                while(1){
                #pragma HLS loop_tripcount min=1 max=8
                    tree_data.compact_data = tree[t][node_index];
                    feature_index = tree_data.tree_camps.feature_index;
                    threshold = tree_data.tree_camps.float_int_union.f;
                    node_left = node_index + 1;
                    node_right = tree_data.tree_camps.next_node_right_index;

                    node_index = *(int32_t*)&local_features_ping[feature_index] < *(int32_t*)&threshold ?
                                            node_left : node_right;

                    if (!(tree_data.tree_camps.leaf_or_node & 0x01))
                        break;
                }

                leaf_value = tree_data.tree_camps.float_int_union.i;;
                sum += leaf_value;
            }

            coppy_loop_pong:for (int i = 0; i < N_FEATURE & j < *features_burst_length - 1; i++)
                local_features_pong[i] = ping_pong & 0x01 ? bram_features_ping[j+1][i] : bram_features_pong[j+1][i];

        }else{

            trees_loop_pong: for (int t = 0; t < N_TREES_IP && t < N_TREES; t++){
            #pragma HLS UNROLL factor=N_TREES_IP
                uint8_t node_index = 0;
                uint8_t node_right;
                uint8_t node_left;
                uint8_t feature_index;
                float threshold;
                tree_data tree_data;

                while(1){
                #pragma HLS loop_tripcount min=1 max=8
                    tree_data.compact_data = tree[t][node_index];
                    feature_index = tree_data.tree_camps.feature_index;
                    threshold = tree_data.tree_camps.float_int_union.f;
                    node_left = node_index + 1;
                    node_right = tree_data.tree_camps.next_node_right_index;

                    node_index = *(int32_t*)&local_features_pong[feature_index] < *(int32_t*)&threshold ?
                                            node_left : node_right;

                    if (!(tree_data.tree_camps.leaf_or_node & 0x01))
                        break;
                }

                leaf_value = tree_data.tree_camps.float_int_union.i;;
                sum += leaf_value;
            }

            coppy_loop_ping:for (int i = 0; i < N_FEATURE & j < *features_burst_length - 1; i++)
                local_features_ping[i] = ping_pong & 0x01 ? bram_features_ping[j+1][i] : bram_features_pong[j+1][i];

        }

        if (ping_pong & 0x01 ){
            prediction_ping[j] = sum;
        }else{
            prediction_pong[j] = sum;
        }

        local_ping_pong = ~local_ping_pong;
    }
    
}
