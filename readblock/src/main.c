#include "readblock.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "quick_sort.h"

/* 1. Лексикографический с учётом регистра (прямой порядок) */
int cmpLexCase(const void *a, const void *b) {
    unsigned char ca = *(const unsigned char *)a;
    unsigned char cb = *(const unsigned char *)b;
    return (ca > cb) - (ca < cb);
}

/* 2. Лексикографический без учёта регистра (прямой порядок) */
int cmpLexNocase(const void *a, const void *b) {
    unsigned char ca = tolower(*(const unsigned char *)a);
    unsigned char cb = tolower(*(const unsigned char *)b);
    return (ca > cb) - (ca < cb);
}

/* 3. Обратный лексикографический без учёта регистра */
int cmpLexReverseNocase(const void *a, const void *b) {
    unsigned char ca = tolower(*(const unsigned char *)a);
    unsigned char cb = tolower(*(const unsigned char *)b);
    return (cb > ca) - (cb < ca);
}

/* 4. По байту (обратный порядок с учётом регистра) */
int cmpLexReverseCase(const void *a, const void *b) {
    unsigned char ca = *(const unsigned char *)a;
    unsigned char cb = *(const unsigned char *)b;
    return (cb > ca) - (cb < ca);
}

/* Запись всех строк (блоков) в выходной файл, каждая строка заканчивается '\n' */

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Использование: %s <входной_файл> <выходной_файл> <номер_компаратора (1-4)>\n", argv[0]);
        return 1;
    }

    const char *inFile  = argv[1];
    const char *outFile = argv[2];
    int cmpNum = strtolSafe(argv[3], nullptr, 10);
    if (cmpNum < 1 || cmpNum > 4) {
        perror("Номер компаратора должен быть 1, 2, 3 или 4");
        return 1;
    }

    /* Чтение файла */
    LargeBlock **blocks = readFile(inFile);
    if (!blocks) {
        perror("Ошибка чтения файла");
        return 1;
    }

    /* Выбор компаратора */
    int (*cmpFunc)(const void *, const void *);
    switch (cmpNum) {
        case 1: cmpFunc = cmpLexCase; break;
        case 2: cmpFunc = cmpLexNocase; break;
        case 3: cmpFunc = cmpLexReverseNocase; break;
        case 4: cmpFunc = cmpLexReverseCase; break;
    }

    /* Сортировка символов внутри каждой строки (до '\0') */
    for (size_t i = 0; blocks[i] != NULL; ++i) {
        char *str = (char *)blocks[i];
        size_t len = strlen(str);
        if (len > 1) {
            quickSort(str, len, sizeof(char), cmpFunc);
        }
    }

    writeBlocks(outFile, blocks);

    /* Освобождение */
    for (size_t i = 0; blocks[i] != NULL; ++i) free(blocks[i]);
    free(blocks);

    return 0;
}
