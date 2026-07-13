/*
Заголовочный файл util.c
Общие утилиты

Кинев Алексей Александрович
МК-102
*/

#pragma once
#include <stddef.h>

void byteToHex(unsigned char b, char *out);

size_t strToSizeT(const char *str, char **endptr);
