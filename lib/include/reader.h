/*
Заголовочный файл reader.c
Чтение данных из файла фиксированными блоками

Кинев Алексей Александрович
МК-102
*/

#pragma once
#include <stddef.h>
#include <stdio.h>

typedef struct Reader {
    FILE* file;
    size_t readSize;
    char *buffer;
} Reader;

Reader* createReader(const char* pathToFile, size_t readSize);

size_t readNext(const Reader* reader);

void destroyReader(Reader* reader);
