#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#include "util.h"

#ifdef _WIN32
  #define FSEEK _fseeki64
#else
  #define FSEEK fseeko
#endif

static constexpr char HELP[] = R"(
    -i file_name	имя выводимого файла
    -o offset		смещение от начала файла, с которого выводится содержимое (если не задано, то выводить с начала файла)
    -l size			количество выводимых из файла байт (если не задано, то выводить всё)
    -g size			размер кусочка (если не задано, то 1)
    -n count		количество кусочков, выводимых в одной строке (если не задано, то 16)
    -d dir			директория, из которой выводить файлы
)";

int myFSeek(FILE *f, size_t pos) {
#ifdef _WIN32
    return _fseeki64(f, (__int64)pos, SEEK_SET);
#else
    return fseeko(f, (off_t)pos, SEEK_SET);
#endif
}

typedef struct Args {
    char* file;
    char* dir;
    size_t offset;
    size_t limit;
    size_t chunkSize;
    size_t chunkCount;
} Args;

Args* readArgs(const int argc, char **argv) {
    int opt;
    opterr = 0;

    Args *args = calloc(1, sizeof(Args));
    if (args == nullptr) return nullptr;

    args->file = nullptr;
    args->dir = nullptr;
    args->limit = (size_t)-1;   // означает "без ограничения"
    args->offset = 0;
    args->chunkSize = 1;
    args->chunkCount = 16;

    char *endptr;
    while ((opt = getopt(argc, argv, "hi:o:l:g:n:d:")) != -1) {
        switch (opt) {
            case 'h':
                puts(HELP);
                free(args);
                exit(0);
            case 'i':
                args->file = optarg;
                break;
            case 'd':
                args->dir = optarg;
                break;
            case 'o': {
                const size_t val = strToSizeT(optarg, &endptr);
                if (optarg == endptr) {
                    fprintf(stderr, "Не удалось распознать offset: %s\n", optarg);
                } else {
                    args->offset = val;
                }
                break;
            }
            case 'l': {
                const size_t val = strToSizeT(optarg, &endptr);
                if (optarg == endptr) {
                    fprintf(stderr, "Не удалось распознать limit: %s\n", optarg);
                } else {
                    args->limit = val;
                }
                break;
            }
            case 'g': {
                const size_t val = strToSizeT(optarg, &endptr);
                if (optarg == endptr) {
                    fprintf(stderr, "Не удалось распознать chunkSize: %s\n", optarg);
                } else if (val == 0) {
                    fprintf(stderr, "Размер кусочка должен быть больше 0\n");
                } else {
                    args->chunkSize = val;
                }
                break;
            }
            case 'n': {
                const size_t val = strToSizeT(optarg, &endptr);
                if (optarg == endptr) {
                    fprintf(stderr, "Не удалось распознать chunkCount: %s\n", optarg);
                } else if (val == 0) {
                    fprintf(stderr, "Количество кусочков в строке должно быть больше 0\n");
                } else {
                    args->chunkCount = val;
                }
                break;
            }
            case '?':
                fprintf(stderr, "Неизвестная опция: -%c\n", optopt);
                break;
            default:
                break;
        }
    }

    // Проверка: должна быть указана ровно одна из опций -i или -d
    if ((args->file != nullptr && args->dir != nullptr) ||
        (args->file == nullptr && args->dir == nullptr)) {
        fprintf(stderr, "Необходимо указать ровно одну из опций: -i или -d\n");
        free(args);
        return nullptr;
    }

    return args;
}

static void printChunkHex(const unsigned char *data, const size_t size) {
    char hex[3] = {0};
    for (size_t i = 0; i < size; i++) {
        byteToHex(data[i], hex);
        fwrite(hex, 1, 2, stdout);
    }
}

static void printChars(const unsigned char *data, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        const unsigned char c = data[i];
        if (c >= 0x20 && c <= 0x7E) {
            putchar(c);
        } else {
            putchar('.');
        }
    }
}

static void processFile(const char *filename, size_t offset, size_t limit,
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

static void processDirectory(const char *dirname, size_t offset, size_t limit,
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

int main(const int argc, char *argv[]) {
    Args *args = readArgs(argc, argv);
    if (args == nullptr)
        return 1;

    if (args->dir != nullptr) {
        processDirectory(args->dir, args->offset, args->limit, args->chunkSize, args->chunkCount);
    } else if (args->file != nullptr) {
        processFile(args->file, args->offset, args->limit, args->chunkSize, args->chunkCount);
    }

    free(args);
    return 0;
}