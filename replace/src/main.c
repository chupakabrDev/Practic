/*
Главный файл приложения replace.
Читает байты из файла, создает новый файл с замененными вхождениями, все аргументы принимаются как program arguments

Кинев Алексей Александрович
МК-102
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

#include "finder.h"
#include "reader.h"
#include "writer.h"

#define N 16

int main(const int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Должно быть 3 аргумента\n");
        return 1;
    }

    int res = 0;

    const char* filename = argv[1];
    const char* original = argv[2];
    const char* replacement = argv[3];

    const size_t replacementLen = strlen(replacement);

    Reader* reader = createReader(filename, N);
    if (reader == nullptr) {
        perror("Ошибка создания Reader");
        return 1;
    }
    Writer* writer = createWriter("output.txt");
    if (writer == nullptr) {
        perror("Ошибка создания Writer");
        res = 1;
        goto cleanupReader;
    }
    Finder* finder = createFinder(original, strlen(original)); // завершающий ноль режется длиной
    if (finder == nullptr) {
        perror("Ошибка создания Finder");
        res = 1;
        goto cleanupWriter;
    }

    size_t count = readNext(reader);
    if (count < N) {
        if (ferror(reader->file)) {
            perror("Ошибка чтения");
            res = 1;
            goto cleanup;
        }
        if (!feof(reader->file)) {
            perror("Пустой файл");
            res = 1;
            goto cleanup;
        }
    }

    size_t last = 0;
    while (count != 0) {
        if (find(finder, reader->buffer, count) == FIND_ERROR) {
            perror("Ошибка поиска");
            res = 1;
            goto cleanup;
        }
        last = 0;

        Match* match;
        while ((match = getMatch(finder)) != nullptr) {
            // запись на граница через реплейс предыдущих байт
            // сократить main

            writeNext(writer, reader->buffer + last, match->start - last);
            writeNext(writer, replacement, replacementLen);
            last = match->end + 1;
            free(match);
        }
        if (finder->currentMatchCount > 0) {
            perror("Ошибка извлечения совпадения");
            res = 1;
            goto cleanup;
        }

        writeNext(writer, reader->buffer + last, count - last);

        count = readNext(reader);
        freeMatches(finder);
    }

    cleanup: {
        destroyFinder(finder);
        cleanupWriter: destroyWriter(writer);
        cleanupReader: destroyReader(reader);
    }

    return res;
}
