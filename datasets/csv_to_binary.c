#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *csvFile, *binFile;
    float num;
    char buffer[1024];

    csvFile = fopen("alzheimers_processed_dataset.csv", "r");
    if (csvFile == NULL) {
        printf("No se pudo abrir el archivo CSV.\n");
        return 1;
    }

    binFile = fopen("alzheimers_processed_dataset.bin", "wb");
    if (binFile == NULL) {
        printf("No se pudo crear el archivo binario.\n");
        fclose(csvFile);
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), csvFile)) {
        char *ptr = buffer;
        
        while (sscanf(ptr, "%f", &num) == 1) {
            fwrite(&num, sizeof(float), 1, binFile);
            
            while (*ptr != ',' && *ptr != '\0') {
                ptr++;
            }
            if (*ptr == ',') {
                ptr++;
            }
        }
    }

    fclose(csvFile);
    fclose(binFile);

    printf("Archivo binario creado exitosamente.\n");
    return 0;
}
