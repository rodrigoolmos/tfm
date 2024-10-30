#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "../linux_drivers/common/common.h"

#define N_TREES 512
#define N_NODE_AND_LEAFS 256
#define N_FEATURE 32

#define N_ITE 400


const char* kernelSource = 
"__kernel void predict_kernel(__global const ulong *tree,"
"                      __global const float *features, "
"                      __global int *prediction) {"
""
"    int t = get_global_id(0);"
""
"    int sum = 0;"
""
"    uchar node_index = 0;"
"    uchar node_right;"
"    uchar node_left;"
"    uchar feature_index;"
"    float threshold;"
"    union {"
"        ulong compact_data;"
"        struct {"
"            uchar leaf_or_node;"
"            uchar feature_index;"
"            uchar next_node_right_index;"
"            uchar padding;"
"            float float_int_union;"
"        } tree_camps;"
"    } tree_data;"
""
"    while (1) {"
"        tree_data.compact_data = tree[t * 256 + node_index];"
"        feature_index = tree_data.tree_camps.feature_index;"
"        threshold = tree_data.tree_camps.float_int_union;"
"        node_left = node_index + 1;"
"        node_right = tree_data.tree_camps.next_node_right_index;"
""
"        node_index = features[feature_index] < threshold ? node_left : node_right;"
""
"        if (!(tree_data.tree_camps.leaf_or_node & 0x01))"
"            break;"
"    }"
""
"    int leaf_value = *((int*)&tree_data.tree_camps.float_int_union);"
"    atomic_add(prediction, leaf_value);"
"}";


void convert_tree_to_vector(uint64_t tree[][N_NODE_AND_LEAFS], size_t n_trees, size_t n_node_and_leafs, uint64_t* vector) {

    for (size_t i = 0; i < n_trees; i++) {
        for (size_t j = 0; j < n_node_and_leafs; j++) {
            vector[i * n_node_and_leafs + j] = tree[i][j];
        }
    }
}

struct gpu_predict
{
	cl_device_id device_id;             
    cl_context context;                 
    cl_command_queue commands;          
    cl_program program;                 
    cl_kernel kernel;

    cl_mem features;
    cl_mem trees;
    cl_mem prediction;
	uint32_t size_features;                 
	uint32_t size_trees;                 
};
struct gpu_predict gpu_predict;

uint32_t init_gpu_predict (){
	
	cl_platform_id platforms;			
	cl_int err; 

    err = clGetPlatformIDs(1, &platforms, NULL);
    err = clGetDeviceIDs(platforms, CL_DEVICE_TYPE_GPU, 1, &(gpu_predict.device_id), NULL);
    if (err != CL_SUCCESS){
        printf("Error: Failed to create a device group!\n");
        return EXIT_FAILURE;
    }
  
    gpu_predict.context = clCreateContext(0, 1, &(gpu_predict.device_id), NULL, NULL, &err);
    if (!gpu_predict.context){
        printf("Error: Failed to create a compute context!\n");
        return EXIT_FAILURE;
    }

    gpu_predict.commands = clCreateCommandQueue(gpu_predict.context, gpu_predict.device_id, 0, &err);
    if (!gpu_predict.commands){
        printf("Error: Failed to create a command commands!\n");
        return EXIT_FAILURE;
    }

    gpu_predict.program = clCreateProgramWithSource(gpu_predict.context, 1, (const char **) & kernelSource, NULL, &err);
    if (!gpu_predict.program){
        printf("Error: Failed to create compute program!\n");
        return EXIT_FAILURE;
    }

    err = clBuildProgram(gpu_predict.program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS){
        size_t len;
        char buffer[2048];

        printf("Error: Failed to build program executable!\n");
        clGetProgramBuildInfo(gpu_predict.program, gpu_predict.device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(1);
    } 

    gpu_predict.kernel = clCreateKernel(gpu_predict.program, "predict_kernel", &err);
    if (!gpu_predict.kernel || err != CL_SUCCESS){
        printf("Error: Failed to create compute kernel!\n");
        exit(1);
    }

    gpu_predict.features  = clCreateBuffer(gpu_predict.context,  CL_MEM_READ_WRITE, sizeof(float) * gpu_predict.size_features,  NULL, NULL);
    gpu_predict.trees  = clCreateBuffer(gpu_predict.context,  CL_MEM_READ_WRITE, sizeof(uint64_t) * gpu_predict.size_trees,  NULL, NULL);
    gpu_predict.prediction  = clCreateBuffer(gpu_predict.context,  CL_MEM_READ_WRITE, sizeof(uint32_t),  NULL, NULL);

	return err;
}

uint32_t execute_gpu_predict (float* features, uint64_t* trees, int32_t* prediction, uint8_t update_trees){

	cl_int err;
	size_t local;  
	size_t global; 
    uint32_t sum = 0;   
 
    if(update_trees)
        err = clEnqueueWriteBuffer(gpu_predict.commands, gpu_predict.trees, CL_TRUE, 0, sizeof(uint64_t) * gpu_predict.size_trees, trees, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(gpu_predict.commands, gpu_predict.features, CL_TRUE, 0, sizeof(float) * gpu_predict.size_features, features, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(gpu_predict.commands, gpu_predict.prediction, CL_TRUE, 0, sizeof(int32_t), prediction, 0, NULL, NULL);
    if (err != CL_SUCCESS){
        printf("Error: Failed to write to source array!\n");
        exit(1);
    }
    if(update_trees)
        err  = clSetKernelArg(gpu_predict.kernel, 0, sizeof(cl_mem), &(gpu_predict.trees));
    err  |= clSetKernelArg(gpu_predict.kernel, 1, sizeof(cl_mem), &(gpu_predict.features));
    err  |= clSetKernelArg(gpu_predict.kernel, 2, sizeof(cl_mem), &(gpu_predict.prediction));
    if (err != CL_SUCCESS){
        printf("Error: Failed to set kernel arguments! %d\n", err);
        exit(1);
    }

    err = clGetKernelWorkGroupInfo(gpu_predict.kernel, gpu_predict.device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to retrieve kernel work group info! %d\n", err);
        exit(1);
    }

    global = N_TREES;
    local = local > global ? global : local;
    err = clEnqueueNDRangeKernel(gpu_predict.commands, gpu_predict.kernel, 1, NULL, &global, &local, 0, NULL, NULL);
    if (err)
    {
        printf("Error: Failed to execute kernel!\n");
        return EXIT_FAILURE;
    }


    err = clEnqueueReadBuffer(gpu_predict.commands, gpu_predict.prediction, CL_TRUE, 0, sizeof(int32_t), prediction, 0, NULL, NULL );  
    if (err != CL_SUCCESS)
    {
        printf("Error: Failed to read output array! %d\n", err);
        exit(1);
    }

	err = clFlush(gpu_predict.commands);
    err = clFinish(gpu_predict.commands);

	return err;
}

uint32_t release_gpu_predict (){

	cl_int err;

    err = clReleaseMemObject(gpu_predict.trees);
    err = clReleaseMemObject(gpu_predict.features);
    err = clReleaseMemObject(gpu_predict.prediction);
    err = clReleaseProgram(gpu_predict.program);
    err = clReleaseKernel(gpu_predict.kernel);
    err = clReleaseCommandQueue(gpu_predict.commands);
    err = clReleaseContext(gpu_predict.context);

	return err;
}

void evaluate_model(uint32_t read_samples, struct feature* features, uint64_t *tree_vector, float* time_used) {

    int accuracy = 0;
    int32_t prediction;
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    for (size_t i = 0; i < read_samples; i++){
        prediction = 0; 
        execute_gpu_predict(features[i].features, tree_vector, &prediction, i==0);
        if (features[i].prediction == (prediction > 0))
            accuracy++;
    }

    printf("Accuracy %f\n", 1.0 * accuracy / read_samples);
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    *time_used = cpu_time_used / read_samples;
    printf("Tiempo de ejecucion por feature: %f segundos\n", *time_used);
}



int main() {
    
    float time_used = 0;
    float time_model_1 = 0;
    float time_model_2 = 0;
    float time_model_3 = 0;
    float time_model_4 = 0;
    float time_model_5 = 0;

    int read_samples;
    int prediction, i;
    uint32_t features_length;

    gpu_predict.size_features = N_FEATURE;
    gpu_predict.size_trees = N_TREES * N_NODE_AND_LEAFS;

    uint64_t tree[N_TREES][N_NODE_AND_LEAFS] __attribute__((aligned(64))) = {0};
    uint64_t tree_vector[N_TREES * N_NODE_AND_LEAFS] __attribute__((aligned(64))) = {0};
    struct feature features[MAX_TEST_SAMPLES] __attribute__((aligned(64))) = {0};

    init_gpu_predict();

    for (i = 0; i < N_ITE; i++){
    
        read_samples = read_n_features("../datasets/diabetes.csv", MAX_TEST_SAMPLES, features, &features_length);
        load_model(tree, "../trained_models/diabetes_512.model");
        convert_tree_to_vector(tree, N_TREES, N_NODE_AND_LEAFS, tree_vector);
        evaluate_model(read_samples, features, tree_vector, &time_used);    
        time_model_1 += time_used;

        read_samples = read_n_features("../datasets/Heart_Attack.csv", MAX_TEST_SAMPLES, features, &features_length);
        load_model(tree, "../trained_models/heart_attack_512.model");
        convert_tree_to_vector(tree, N_TREES, N_NODE_AND_LEAFS, tree_vector);
        evaluate_model(read_samples, features, tree_vector, &time_used);    
        time_model_2 += time_used;

        read_samples = read_n_features("../datasets/Lung_Cancer_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
        load_model(tree, "../trained_models/lung_cancer_512.model");
        convert_tree_to_vector(tree, N_TREES, N_NODE_AND_LEAFS, tree_vector);
        evaluate_model(read_samples, features, tree_vector, &time_used);    
        time_model_3 += time_used;

        read_samples = read_n_features("../datasets/anemia_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
        load_model(tree, "../trained_models/anemia_512.model");
        convert_tree_to_vector(tree, N_TREES, N_NODE_AND_LEAFS, tree_vector);
        evaluate_model(read_samples, features, tree_vector, &time_used);    
        time_model_4 += time_used;

        read_samples = read_n_features("../datasets/alzheimers_processed_dataset.csv", MAX_TEST_SAMPLES, features, &features_length);
        load_model(tree, "../trained_models/alzheimers_512.model");
        convert_tree_to_vector(tree, N_TREES, N_NODE_AND_LEAFS, tree_vector);
        evaluate_model(read_samples, features, tree_vector, &time_used);    
        time_model_5 += time_used;

        }

    release_gpu_predict();

    printf("AVERAGE TIME GPU MODEL 1 %f\n", time_model_1 / N_ITE);
    printf("AVERAGE TIME GPU MODEL 2 %f\n", time_model_2 / N_ITE);
    printf("AVERAGE TIME GPU MODEL 3 %f\n", time_model_3 / N_ITE);
    printf("AVERAGE TIME GPU MODEL 4 %f\n", time_model_4 / N_ITE);
    printf("AVERAGE TIME GPU MODEL 5 %f\n", time_model_5 / N_ITE);

    return 0;
}
