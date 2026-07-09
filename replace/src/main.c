/*
Главный файл приложения replace.
Читает байты из файла, создает новый файл с замененными вхождениями, все аргументы принимаются как program arguments

Кинев Алексей Александрович
МК-102
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "finder.h"
#include "reader.h"
#include "writer.h"

#define N 16

int main(const int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Должно быть 3 аргумента\n");
        return 1;
    }

    const char* filename = argv[1];
    const char* original = argv[2];
    const char* replacement = argv[3];

    const size_t replacementLen = strlen(replacement);

    Reader* reader = createReader(filename, N);
    Writer* writer = createWriter("output.txt");
    Finder* finder = createFinder(original, strlen(original)); // нулевой бат не берется потому что длина его не учитывает

    size_t count = readNext(reader);
    if (count == 0) {
        puts("Пустой файл");
        destroyReader(reader);
        goto destroy;
    }

    size_t last = 0;
    while (count != 0) {
        find(finder, reader->buffer, count);

        Match* match = getMatch(finder);
        while (match != nullptr) {
            writeNext(writer, reader->buffer + last, match->start - last); // до совпадения
            writeNext(writer, replacement, replacementLen); // совпадение
            last = match->end + 1;

            free(match);
            match = getMatch(finder);
        }
        writeNext(writer, reader->buffer + last, count - last);

        count = readNext(reader);
        freeMatches(finder);
    }

    destroy: {
        destroyReader(reader);
        destroyWriter(writer);
        destroyFinder(finder);
    }

    return 0;
}
