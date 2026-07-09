/*
Заголовочный файл writer.c
Оболочка над fwrite

Кинев Алексей Александрович
МК-102
*/

#pragma once
#include <stdio.h>

typedef struct Writer {
    FILE* file;
} Writer;

Writer* createWriter(const char* filename);

size_t writeNext(const Writer* writer, const char* data, size_t size); // вернет кол-во записанных байт

void destroyWriter(Writer* writer);
