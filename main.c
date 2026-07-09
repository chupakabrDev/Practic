#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "finder.h"
#include "reader.h"

#define N 16

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Должно быть 3 аргумента\n");
        return 1;
    }

    char* filename = argv[1];
    char* original = argv[2];
    char* replacement = argv[3];

ptrdiff_t offset = (ptrdiff_t)strlen(original) - (ptrdiff_t)strlen(replacement);    printf("%td\n", offset);

    Reader* reader = createReader(filename, N);
    Finder* finder = createFinder(original, strlen(original)); // нулевой бат не берется потому что длина его не учитывает

    size_t count = readNext(reader);
    if (count == 0) {
        puts("Пустой файл");
        destroyReader(reader);
        goto destroy;
    }

    while (count != 0) {

        if (find(finder, reader->buffer, count)) {
            Match* match = get(finder);
            while (match != nullptr) {
                printf("start: %lu; end: %lu\n", match->start, match->end);
                match = get(finder);
            }
            free(match);
        }

        count = readNext(reader);
    }

    destroy: {
        destroyReader(reader);
        destroyFinder(finder);
    }

    return 0;
}
