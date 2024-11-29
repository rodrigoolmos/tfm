#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_TEST_SAMPLES 30000

#define N_NODE_AND_LEAFS 256
#define N_TREES 128
#define N_FEATURE 32

/**
 * @brief Represents a set of features along with the prediction result.
 * 
 * This structure holds an array of features and a prediction result. It is used for storing the feature
 * data of a sample and the corresponding prediction made by a model.
 * 
 * @var feature::features
 * Array containing the feature values for a given sample. The size of the array is N_FEATURE.
 * 
 * @var feature::prediction
 * The prediction result for the sample. It is represented as an 8-bit unsigned integer.
 */
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

/**
 * @brief Loads a trained model from a file into the tree data structure.
 * 
 * This function reads a trained model from a specified file and populates the tree data structure
 * with the model's parameters, including nodes and leaf information for each tree.
 * 
 * @param tree_data   A 2D array containing the tree model data, where each element holds the information 
 *                    for the nodes and leaves of a tree. The array size is N_TREES by N_NODE_AND_LEAFS.
 * @param filename    The path to the file containing the trained model to be loaded.
 */
void load_model(tree_data tree_data[N_TREES][N_NODE_AND_LEAFS], const char *filename);