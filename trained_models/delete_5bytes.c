#include <stdio.h>
#include <stdlib.h>

void eliminarPrimeros5Bytes(const char *nombreArchivoOriginal, const char *nombreArchivoNuevo) {
    FILE *archivoOriginal = fopen(nombreArchivoOriginal, "rb");
    if (archivoOriginal == NULL) {
        perror("Error al abrir el archivo original");
        return;
    }

    // Abrir el nuevo archivo en modo binario
    FILE *archivoNuevo = fopen(nombreArchivoNuevo, "wb");
    if (archivoNuevo == NULL) {
        perror("Error al abrir el archivo nuevo");
        fclose(archivoOriginal);
        return;
    }

    // Mover el puntero del archivo al byte 5
    fseek(archivoOriginal, 5, SEEK_SET);

    // Copiar el resto del archivo al nuevo archivo
    char buffer[1024];
    size_t bytesLeidos;
    while ((bytesLeidos = fread(buffer, 1, sizeof(buffer), archivoOriginal)) > 0) {
        fwrite(buffer, 1, bytesLeidos, archivoNuevo);
    }

    // Cerrar los archivos
    fclose(archivoOriginal);
    fclose(archivoNuevo);

    printf("Se han eliminado los primeros 5 bytes y el resto se ha guardado en %s\n", nombreArchivoNuevo);
}

int main() {
    const char *nombreArchivoOriginal = "alzheimers.model_tb";
    const char *nombreArchivoNuevo = "alzheimer.model_tb";

    eliminarPrimeros5Bytes(nombreArchivoOriginal, nombreArchivoNuevo);

    return 0;
}
