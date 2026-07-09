/*
Реализация reader.h

Кинев Алексей Александрович
МК-102
*/

#include "reader.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>

Reader* createReader(const char *pathToFile, const size_t readSize) {
    FILE* file = fopen(pathToFile, "rb"); // rb более точный, r преобразовывает \r\n -> \n
    checkOpenFile(file);

    Reader *reader = malloc(sizeof(Reader));
    checkAllocateMem(reader);

    reader->file = file;
    reader->readSize = readSize > 0 ? readSize : 1;
    reader->buffer = calloc(1, reader->readSize); // чтобы обнулить буфер
    checkAllocateMem(reader->buffer);

    return reader;
}

// ввернет кол-во прочитанных байт
size_t readNext(const Reader *reader) {
    if (reader == nullptr) return 0;

    memset(reader->buffer, 0, reader->readSize);
    return fread(reader->buffer, 1, reader->readSize, reader->file);
}

void destroyReader(Reader *reader) {
    if (reader == nullptr) return;

    fclose(reader->file);
    reader->file = nullptr;

    free(reader->buffer);
    reader->buffer = nullptr;

    free(reader);
}
