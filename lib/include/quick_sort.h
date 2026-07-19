#pragma once
#include <stddef.h>

void quickSort(void* arr, size_t count, size_t size, int (*comparator)(const void*, const void*));
