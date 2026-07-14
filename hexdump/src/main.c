/*
Главный файл подпроекта hexdump, точка входа

Кинев Алексей Александрович
МК-102
*/
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "util.h"
#include "hexdump.h"

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