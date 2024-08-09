#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "defines_dma.h" 

void write_to_bram(int fd, off_t base_addr, const void *data, size_t size) {
    if (pwrite(fd, data, size, base_addr) == -1) {
        perror("pwrite");
        exit(EXIT_FAILURE);
    }
}

void read_from_bram(int fd, off_t base_addr, void *buffer, size_t size) {
    if (pread(fd, buffer, size, base_addr) == -1) {
        perror("pread");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int fd_h2c = open("/dev/xdma0_h2c_0", O_RDWR);
    if (fd_h2c == -1) {
        perror("open /dev/xdma0_h2c_0");
        return EXIT_FAILURE;
    }

    int fd_c2h = open("/dev/xdma0_c2h_0", O_RDWR);
    if (fd_c2h == -1) {
        perror("open /dev/xdma0_c2h_0");
        close(fd_h2c);
        return EXIT_FAILURE;
    }

    /* Datos para escribir en las BRAM */
    unsigned long int trees_wr[TREES_SIZE / sizeof(unsigned long int)];
    unsigned long int features_wr[FEATURES_SIZE / sizeof(unsigned long int)];

    /* Buffers para lectura */
    unsigned long int trees_read[TREES_SIZE / sizeof(unsigned long int)];
    unsigned int features_read[FEATURES_SIZE / sizeof(unsigned int)];

    int i;

    /* Llenar las BRAM con valores consecutivos en hexadecimal */
    for (i = 0; i < TREES_SIZE / sizeof(unsigned long int); ++i) {
        trees_wr[i] = i;
    }
    // for (i = 0; i < FEATURES_SIZE / sizeof(unsigned int); ++i) {
    //     features_wr[i] = i + 10;
    // }

    write_to_bram(fd_h2c, TREES_ADDR, trees_wr, sizeof(trees_wr));

    //write_to_bram(fd_h2c, FEATURES_ADDR, features_wr, sizeof(features_wr));

    read_from_bram(fd_c2h, TREES_ADDR, trees_read, sizeof(trees_read));

    //read_from_bram(fd_c2h, FEATURES_ADDR, features_read, sizeof(features_read));


    close(fd_h2c);
    close(fd_c2h);

    return 0;
}