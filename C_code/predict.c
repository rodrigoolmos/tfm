#include "predict.h"

uint8_t cmpf(float a, float b) {
    // Convert the floats to unsigned 32-bit integers
    uint32_t int_a = *(uint32_t*)&a;
    uint32_t int_b = *(uint32_t*)&b;

    // Extract the sign bits, exponents, and mantissas
    uint8_t sign_a = int_a >> 31;
    uint8_t sign_b = int_b >> 31;
    uint32_t abs_a = int_a & 0x7FFFFFFF;
    uint32_t abs_b = int_b & 0x7FFFFFFF;

    // Compare the sign bits
    if (sign_a > sign_b) {
        return 1;
    }

    // Compare the absolute values considering the sign
    uint8_t temp = (abs_a > abs_b);
    return (!sign_a) ? !temp : temp;
}

void predict(float_int_union node_leaf_value[N_TREES][N_NODE_AND_LEAFS],
            uint8_t compact_data[N_TREES][N_NODE_AND_LEAFS],
            uint8_t next_node_right_index[N_TREES][N_NODE_AND_LEAFS],
            float features[N_FEATURE], uint8_t *prediction){

	#pragma HLS INTERFACE mode=s_axilite port=features
	#pragma HLS INTERFACE mode=s_axilite port=next_node_right_index
	#pragma HLS INTERFACE mode=s_axilite port=compact_data
	#pragma HLS INTERFACE mode=s_axilite port=node_leaf_value
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=next_node_right_index
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=compact_data
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=node_leaf_value
	#pragma HLS ARRAY_PARTITION dim=1 factor=N_FEATURE/2 type=cyclic variable=features

	int32_t sum = 0;
    int32_t leaf_value;
    float features_local[N_FEATURE];

    coppy_features:for (int f = 0; f < N_FEATURE; f++) {
        features_local[f] = features[f];
    }


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
            threshold = node_leaf_value[t][node_index].f;
            node_left = node_index + 1;
            node_right = next_node_right_index[t][node_index];

            node_index = cmpf(features_local[feature_index], threshold) ? node_left : node_right;

            done = (compact_data[t][node_index] & 0x80) == 0 ? 1 : 0;

           if (done)
                break;
        }

        leaf_value = node_leaf_value[t][node_index].i;
        sum += leaf_value;
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
