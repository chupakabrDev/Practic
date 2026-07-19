#include "quick_sort.h"

#include "util.h"

static int partition(void* arr, const int start, const int end, const size_t size, int (*comparator)(const void*, const void*)) {
    // Опорный элемент – последний
    char* pivot = (char*)arr + end * size;
    int i = start - 1;

    for (int j = start; j < end; j++) {
        char* cur = (char*)arr + j * size;
        // Если текущий элемент меньше опорного
        if (comparator(cur, pivot) < 0) {
            i++;
            char* left = (char*)arr + i * size;
            swap(left, cur, size);
        }
    }

    // Ставим опорный элемент на своё место (после всех меньших)
    char* left = (char*)arr + (i + 1) * size;
    swap(left, pivot, size);
    return i + 1;
}

static void quick(void* arr, const int start, const int end, const size_t size, int (*comparator)(const void*, const void*)) {
    if (start >= end) return;

    const int p = partition(arr, start, end, size, comparator);
    quick(arr, start, p - 1, size, comparator);
    quick(arr, p + 1, end, size, comparator);
}


void quickSort(void* arr, const size_t count, const size_t size, int (*comparator)(const void*, const void*)) {
    if (count <= 1) return;

    quick(arr, 0, (int)count - 1, size, comparator);
}
