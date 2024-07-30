#include "predict.h"

void predict(struct tree trees[], float features[], uint8_t *prediction) {
    float sum = 0.0;

    for (int t = 0; t < N_TREES; t++) {
        int node_index = 0;

        while (trees[t].leaf_or_node[node_index] == 1) {
            int feature_index = trees[t].feature_index[node_index];
            float threshold = trees[t].node_leaf_value[node_index];
            if (features[feature_index] <= threshold) {
                node_index += 1;
            } else {
                node_index = trees[t].next_node_right_index[node_index];
            }
        }
        sum += trees[t].node_leaf_value[node_index];
    }
    *prediction = (uint8_t)(sum > 0 ? 1 : 0);
}