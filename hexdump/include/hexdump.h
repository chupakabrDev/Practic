/*
Заголовочный файл hexdump.c
Обработка файла и директории для шестнадцатеричного вывода содержимого, утилиты

Кинев Алексей Александрович
МК-102
*/
#pragma once

#include <stddef.h>
#include <stdio.h>

int myFSeek(FILE *f, size_t pos);


void printChunkHex(const unsigned char *data, size_t size);


void printChars(const unsigned char *data, size_t size);


void processFile(const char *filename,
                     size_t offset,
                     size_t limit,
                     size_t chunkSize,
                     size_t chunkCount);


void processDirectory(const char *dirname,
                          size_t offset,
                          size_t limit,
                          size_t chunkSize,
                          size_t chunkCount);
