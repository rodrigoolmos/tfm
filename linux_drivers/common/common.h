#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_TEST_SAMPLES 3000

#define N_NODE_AND_LEAFS 256
#define N_TREES 64
#define N_FEATURE 32

struct feature {
    float features[N_FEATURE];
    uint8_t prediction;
};

typedef union {
    float f;
    int32_t i;
} float_int_union;

struct tree_camps {
    uint8_t leaf_or_node;
    uint8_t feature_index;
    uint8_t next_node_right_index;
    uint8_t padding;
    float_int_union float_int_union;
};

typedef union {
    struct tree_camps tree_camps;
    uint64_t compact_data;
} tree_data;


int read_n_features(const char *csv_file, int n, struct feature *features, uint32_t *features_length);

void load_model(tree_data tree_data[N_TREES][N_NODE_AND_LEAFS], const char *filename);