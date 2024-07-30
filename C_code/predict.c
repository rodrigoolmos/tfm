#include "predict.h"

void predict(struct tree trees[N_TREES], float features[N_FEATURE], uint8_t *prediction) {
    int32_t sum = 0;

    trees_loop:for (int t = 0; t < N_TREES; t++) {
        int node_index = 0;
        int feature_index;
        float threshold;

        while (trees[t].leaf_or_node[node_index] == 1) {
            #pragma HLS loop_tripcount min=1 max=100
            feature_index = trees[t].feature_index[node_index];
            threshold = trees[t].node_leaf_value[node_index];

            node_index = (features[feature_index] <= threshold) ? node_index + 1 : trees[t].next_node_right_index[node_index];
        }
        sum += trees[t].node_leaf_value[node_index];
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}
