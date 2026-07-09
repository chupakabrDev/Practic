#include "test.h"

#include <stdlib.h>

#include "finder.h"
#include "reader.h"

void testReader() {
    puts("Начало теста Reader");

    Reader* reader = createReader("/home/aleksey/CLionProjects/Practic/test.txt", 1);

    size_t count = readNext(reader);
    if (count == 0) {
        puts("Пустой файл");
        destroyReader(reader);
        return;
    }

    while (count != 0) {

        char *p = reader->buffer;
        while (count--) {
            putchar(*p++);
            putchar('\n');
        }

        count = readNext(reader);
    }

    destroyReader(reader);
    puts("Конец теста Reader");
}

void testFinder() {
    puts("Начало теста Finder");

    constexpr char target[] = {'a', 'b', 'a'};
    constexpr char inputa[] = {'a', 'b'};
    constexpr char inputb[] = {'a', 'b'};
    constexpr char inputc[] = {'a', 'b'};
    constexpr char inputd[] = {'c'};

    Finder* finder = createFinder(target, 3);

    if (find(finder, inputa, 2)) {
        Match* match = getMatch(finder);
        while (match != nullptr) {
            printf("start: %lu; end: %lu\n", match->start, match->end);
            match = getMatch(finder);
        }
        free(match);
    }

    if (find(finder, inputb, 2)) {
        Match* match = getMatch(finder);
        while (match != nullptr) {
            printf("start: %lu; end: %lu\n", match->start, match->end);
            match = getMatch(finder);
        }
        free(match);
    }

    if (find(finder, inputc, 2)) {
        Match* match = getMatch(finder);
        while (match != nullptr) {
            printf("start: %lu; end: %lu\n", match->start, match->end);
            match = getMatch(finder);
        }
        free(match);
    }

    if (find(finder, inputd, 1)) {
        Match* match = getMatch(finder);
        while (match != nullptr) {
            printf("start: %lu; end: %lu\n", match->start, match->end);
            match = getMatch(finder);
        }
        free(match);
    }

    destroyFinder(finder);

    puts("Конец теста Finder");
}
