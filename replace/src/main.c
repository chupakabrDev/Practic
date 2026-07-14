#include <stdio.h>
#include <stdlib.h>
#include "replace.h"

#define BLOCK_SIZE 1

int main(const int argc, char* argv[]) {
    if (argc != 5) {
        perror("Должно быть 5 аргументов");
        return EXIT_FAILURE;
    }

    const char* inputPath  = argv[1];
    const char* outputPath = argv[2];
    const char* patHex     = argv[3];
    const char* repHex     = argv[4];

    return replace(inputPath, outputPath, patHex, repHex, BLOCK_SIZE);
}