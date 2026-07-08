#pragma once
#include <stddef.h>
#include <stdio.h>

typedef struct Reader {
    FILE* file;
    size_t size;
    char *buffer;
} Reader;

Reader* createReader(const char* pathToFile, size_t readSize);

size_t readNext(const Reader* reader);

void destroyReader(Reader* reader);
