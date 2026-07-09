/*
Заголовочный файл util.c
Общие утилиты

Кинев Алексей Александрович
МК-102
*/

#pragma once

void exitIfNull(const void* ptr, const char* msg);

void checkAllocateMem(const void* ptr);

void checkOpenFile(const void* ptr);
