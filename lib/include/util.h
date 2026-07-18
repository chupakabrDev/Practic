/*
Заголовочный файл util.c
Общие утилиты

Кинев Алексей Александрович
МК-102
*/

#pragma once
#include <stddef.h>

unsigned char hexToNibble(char c);

unsigned char* hexToBytes(const char* hex, size_t* outLen);

void byteToHex(unsigned char b, char *out);

size_t strToSizeT(const char *str, char **endptr);

bool startsWith(const char *str, const char *prefix);

void putError(const char *str, char **error);
