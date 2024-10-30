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

/**
 * @brief Evaluates the performance of a previously trained and exported model on an FPGA.
 * 
 * @param fd_h2c        File descriptor for host-to-channel communication.
 * @param fd_c2h        File descriptor for channel-to-host communication.
 * @param tree_data     Model previously trained and exported in the specific format (.model) using Python or a custom training tool.
 * @param fd_user       File descriptor for configuration.
 * @param features      Structure containing feature data and prediction results.
 * @param raw_features  Matrix with the raw feature data.
 * @param inference     Results of the inferences performed by the model.
 * @param read_samples  Number of feature sets to process.
 */
void evaluate_model(int fd_h2c, int fd_c2h, tree_data tree_data[N_TREES][N_NODE_AND_LEAFS],
                    int fd_user, struct feature features[MAX_TEST_SAMPLES], uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE],
                    int32_t inference[MAX_TEST_SAMPLES], uint32_t read_samples, float* time_execution);
                    
/**
 * @brief Loads a CSV file into the features structure and the matrix of raw features.
 * 
 * @param filename        Path to the CSV file (e.g., "path_to_csc/data.csv").
 * @param max_test_samples Maximum number of test samples, defined by MAX_TEST_SAMPLES.
 * @param features        Structure that holds the features.
 * @param raw_features    Matrix containing raw feature data.
 * @param features_length Number of features per sample.
 * @param read_samples    Number of samples readed from the file.
 */
void load_features(const char* filename, int max_test_samples, struct feature* features,
                uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int* features_length, int* read_samples);

/**
 * @brief Performs a burst ping-pong process to handle raw feature data and generate inferences on FPGA.
 *
 * This function manages the transfer of raw feature data to the FPGA through a burst ping-pong process
 * and retrieves inference results. The process is done through memory-mapped resources and communication
 * channels between the host and FPGA.
 * 
 * @param fd_user          File descriptor for configuration.
 * @param fd_h2c           File descriptor for host-to-channel communication (sending data from host to FPGA).
 * @param fd_c2h           File descriptor for channel-to-host communication (receiving data from FPGA to host).
 * @param raw_features     Matrix containing the raw features to be processed.
 * @param n_features_total Total number of features across all samples.
 * @param inference        Pointer to an array where the inference results will be stored after processing.
 */
void burst_ping_pong_process(int fd_user, int fd_h2c, int fd_c2h, 
                    uint32_t raw_features[MAX_TEST_SAMPLES][N_FEATURE], int32_t n_features_total, int32_t* inference);
