/*
Реализация hexdump.c

Кинев Алексей Александрович
МК-102
*/
#include "hexdump.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "util.h"

int myFSeek(FILE *f, size_t pos) {
#ifdef _WIN32
    return _fseeki64(f, (__int64)pos, SEEK_SET);
#else
    return fseeko(f, (off_t)pos, SEEK_SET);
#endif
}

void printChunkHex(const unsigned char *data, const size_t size) {
    char hex[3] = {0};
    for (size_t i = 0; i < size; i++) {
        byteToHex(data[i], hex);
        fwrite(hex, 1, 2, stdout);
    }
}

void printChars(const unsigned char *data, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        const unsigned char c = data[i];
        if (c >= 0x20 && c <= 0x7E) {
            putchar(c);
        } else {
            putchar('.');
        }
    }
}

void processFile(const char *filename, size_t offset, size_t limit,
                        size_t chunkSize, size_t chunkCount) {
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Не удалось открыть файл '%s': %s\n", filename, strerror(errno));
        return;
    }

    if (fseek(f, 0, SEEK_END) != 0) {
        fprintf(stderr, "Ошибка fseek: %s\n", strerror(errno));
        fclose(f);
        return;
    }
    const long fileSizeLong = ftell(f);
    if (fileSizeLong < 0) {
        fprintf(stderr, "Ошибка ftell: %s\n", strerror(errno));
        fclose(f);
        return;
    }
    const size_t fileSize = (size_t)fileSizeLong;

    if (offset >= fileSize) {
        fclose(f);
        return;
    }

    if (myFSeek(f, offset) != 0) {
        fprintf(stderr, "Ошибка fseek к смещению %zu: %s\n", offset, strerror(errno));
        fclose(f);
        return;
    }

    size_t bytesToRead = fileSize - offset;
    if (limit != (size_t)-1 && limit < bytesToRead) {
        bytesToRead = limit;
    }

    const size_t lineBufSize = chunkCount * chunkSize;
    unsigned char *lineBuf = malloc(lineBufSize);
    if (!lineBuf) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        fclose(f);
        return;
    }

    size_t bytesReadTotal = 0;
    size_t currentOffset = offset;   // смещение для текущей строки

    while (bytesReadTotal < bytesToRead) {
        // набираем байты для строки
        size_t bytesInLine = 0;
        const size_t bytesAvailable = bytesToRead - bytesReadTotal;

        // читаем максимум lineBufSize байт, но не более available
        size_t toRead = (bytesAvailable < lineBufSize) ? bytesAvailable : lineBufSize;
        size_t read = fread(lineBuf, 1, toRead, f);
        if (read == 0) break;

        bytesReadTotal += read;
        bytesInLine = read;

        // Выводим смещение текущей строки (в hex, 8 цифр)
        printf("%08zX  ", currentOffset);
        currentOffset += bytesInLine;

        // Выводим кусочки
        size_t pos = 0;
        while (pos < bytesInLine) {
            const size_t chunkBytes = (bytesInLine - pos) < chunkSize ? (bytesInLine - pos) : chunkSize;
            printChunkHex(lineBuf + pos, chunkBytes);
            pos += chunkBytes;

            if (pos < bytesInLine) {
                putchar(' ');
            }
        }

        if (chunkSize == 1) {
            printf("  ");
            printChars(lineBuf, bytesInLine);
        }

        putchar('\n');
    }

    free(lineBuf);
    fclose(f);
}

void processDirectory(const char *dirname, size_t offset, size_t limit,
                             size_t chunkSize, size_t chunkCount) {
    DIR *dir = opendir(dirname);
    if (!dir) {
        fprintf(stderr, "Не удалось открыть директорию '%s': %s\n", dirname, strerror(errno));
        return;
    }

    struct dirent *entry;
    struct stat st;
    char fullPath[PATH_MAX];

    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirname, entry->d_name);
        if (stat(fullPath, &st) != 0) {
            fprintf(stderr, "stat ошибка для '%s': %s\n", fullPath, strerror(errno));
            continue;
        }

        // Выводим только обычные файлы
        if (S_ISREG(st.st_mode)) {
            printf("\n=== %s ===\n", entry->d_name);
            processFile(fullPath, offset, limit, chunkSize, chunkCount);
        }
    }
    closedir(dir);
}