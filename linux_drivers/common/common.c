#include "common.h"

void load_model(
            tree_data tree_data[N_TREES][N_NODE_AND_LEAFS],
            const char *filename) {

    char magic_number[5] = {0};
    int t, n;
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening the file %s\n", filename);
        perror("Failed to open the file");
        exit(1);
    }

    fread(magic_number, 5, 1, file);

    if (!memcmp(magic_number, "model", 5)){
        for (t = 0; t < N_TREES; t++) {
            for (n = 0; n < N_NODE_AND_LEAFS; n++) {
                fread(&tree_data[t][n], sizeof(uint64_t), 1, file);
            }
        }
    }else{
        perror("Unknown file type");
        exit(1);
    }

    fclose(file);
}

int read_n_features(const char *csv_file, int n, struct feature *features, uint32_t *features_length) {
    FILE *file = fopen(csv_file, "r");
    char line[MAX_LINE_LENGTH];
    int features_read = 0;
    int index;
    int i;

    if (!file) {
        printf("Incorrect path %s\n", csv_file);
        perror("Failed to open the file");
        exit(1);
    }

    while (fgets(line, MAX_LINE_LENGTH, file) && features_read < n) {
        float temp[N_FEATURE + 1];
        char *token = strtok(line, ",");
        index = 0;

        while (token != NULL && index < N_FEATURE + 1) {
            temp[index] = atof(token);
            token = strtok(NULL, ",");
            index++;
        }

        for (i = 0; i < index - 1; i++) {
            features[features_read].features[i] = temp[i];
        }
        features[features_read].prediction = (uint8_t) temp[index - 1];

        features_read++;
    }

    *features_length = index;

    fclose(file);
    return features_read;
}