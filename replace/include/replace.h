/*
Заголовочный файл replace.c
Замена вхождений байт в файле

Кинев Алексей Александрович
МК-102
*/
#pragma once
#include <stddef.h>

int replace(const char* inputPath, const char* outputPath, const char* patHex, const char* repHex, size_t blockSize);
