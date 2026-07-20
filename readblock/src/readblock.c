#include "readblock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* readLine(FILE *f) {
    if (!f) return nullptr;

    char *buffer = malloc(1);
    if (!buffer) {
        fprintf(stderr, "Ошибка: сбой malloc в readLine\n");
        return nullptr;
    }

    size_t capacity = 1;
    size_t length = 0;

    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (ch == '\n') {
            buffer[length] = '\0';
            return buffer;
        }

        if (length + 1 >= capacity) {
            size_t newCapacity = capacity * 2;
            char *newBuffer = malloc(newCapacity);
            if (!newBuffer) {
                fprintf(stderr, "Ошибка: сбой malloc при расширении в readLine\n");
                free(buffer);
                return nullptr;
            }
            memcpy(newBuffer, buffer, length);
            free(buffer);
            buffer = newBuffer;
            capacity = newCapacity;
        }

        buffer[length++] = (char)ch;
    }

    // Если достигнут конец файла и ничего не прочитано
    if (length == 0) {
        free(buffer);
        return nullptr;
    }

    buffer[length] = '\0';
    return buffer;
}

static LargeBlock* splitLineToBlocks(const char *line) {
    if (!line) return nullptr;

    size_t lineLen = strlen(line);

    // Количество блоков, необходимых для хранения строки
    size_t blockCount = (lineLen + LARGE_BLOCK_SIZE - 1) / LARGE_BLOCK_SIZE;
    if (blockCount == 0) blockCount = 1; // для пустой строки

    // Выделяем память под блоки + один нулевой блок-терминатор
    LargeBlock *blocks = malloc((blockCount + 1) * LARGE_BLOCK_SIZE);
    if (!blocks) {
        fprintf(stderr, "Ошибка: сбой malloc для блоков\n");
        return nullptr;
    }

    // Обнуляем всю выделенную область (включая терминатор)
    memset(blocks, 0, (blockCount + 1) * LARGE_BLOCK_SIZE);

    // Копируем содержимое строки в начало выделенной области
    if (lineLen > 0) {
        memcpy(blocks, line, lineLen);
    }

    // Возвращаем указатель на первый блок; последний блок (индекс blockCount) будет нулевым
    return blocks;
}


static LargeBlock** addString(LargeBlock **strings, LargeBlock *newLine) {
    if (!newLine) return strings;

    // Подсчёт текущего количества строк
    size_t count = 0;
    if (strings) {
        while (strings[count] != NULL) {
            count++;
        }
    }

    // Выделяем новый массив указателей (count + 2: существующие + новая + NULL)
    LargeBlock **newStrings = malloc((count + 2) * sizeof(LargeBlock*));
    if (!newStrings) {
        fprintf(stderr, "Ошибка: сбой malloc для расширения массива строк\n");
        free(newLine);  // избегаем утечки
        return nullptr;
    }

    // Копируем старые указатели
    if (strings) {
        memcpy(newStrings, strings, count * sizeof(LargeBlock*));
        free(strings);
    }

    // Добавляем новую строку и завершающий NULL
    newStrings[count] = newLine;
    newStrings[count + 1] = nullptr;

    return newStrings;
}

LargeBlock** readFile(const char *filename) {
    if (!filename) {
        fprintf(stderr, "Ошибка: имя файла равно NULL\n");
        return nullptr;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Ошибка: fopen не удался");
        return nullptr;
    }

    LargeBlock **result = nullptr;
    char *line;

    while ((line = readLine(file)) != NULL) {
        LargeBlock *blocks = splitLineToBlocks(line);
        free(line);  // строка больше не нужна

        if (!blocks) {
            fprintf(stderr, "Ошибка: не удалось разбить строку на блоки\n");
            // Освобождаем всё, что успели накопить
            if (result) {
                for (size_t i = 0; result[i] != NULL; ++i) {
                    free(result[i]);
                }
                free(result);
            }
            fclose(file);
            return nullptr;
        }

        LargeBlock **newResult = addString(result, blocks);
        if (!newResult) {
            fprintf(stderr, "Ошибка: не удалось добавить строку в результат\n");
            free(blocks);  // блоки не были добавлены
            if (result) {
                for (size_t i = 0; result[i] != NULL; ++i) {
                    free(result[i]);
                }
                free(result);
            }
            fclose(file);
            return nullptr;
        }

        result = newResult;
    }

    fclose(file);
    return result;
}

void writeBlocks(const char *file, LargeBlock **strings) {
    if (!file || !strings) return;

    FILE *out = fopen(file, "w");
    if (!out) {
        perror("Ошибка открытия файла: writeBlocks");
        return;
    }

    for (size_t i = 0; strings[i] != NULL; i++) {
        const char *str = (const char *)strings[i];
        size_t len = strlen(str);
        fwrite(str, 1, len, out);
        fputc('\n', out);
    }
}