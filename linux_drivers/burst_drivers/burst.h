#include "../common/common.h"

#define MAP_SIZE 0x20000

#define PING 0xffffffff
#define PONG 0x00000000

#define FEATURES_PING_ADDR 0xC0000000
#define FEATURES_PONG_ADDR 0xC2000000
#define PREDICTIONS_PING_ADDR 0xC4000000
#define PREDICTIONS_PONG_ADDR 0xC6000000

#define TREES_ADDR 0x00800

#define CONTROL_ADDR_BASE 0x0
#define CONTROL_ADDR CONTROL_ADDR_BASE + 0
#define BURST_LENGTH_ADDR CONTROL_ADDR_BASE + 0x10
#define PING_PONG_ADDR CONTROL_ADDR_BASE + 0x18

#define MAX_BURST 256

void evaluate_model(int fd_h2c, int fd_c2h, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS],
                    void *map_base, struct feature features[MAX_TEST_SAMPLES], uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE],
                    int32_t inference[MAX_TEST_SAMPLES], uint32_t read_samples);
                    
void load_features(const char* filename, int max_test_samples, struct feature* features,
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int* features_length, int* read_samples);
