/*
Заголовочный файл finder.c
Поиск вхождений в потоке по алгоритму Кнута-Морриса-Пратта, без учета перекрытий

Кинев Алексей Александрович
МК-102
*/

#pragma once
#include <stddef.h>

typedef enum FindResult {
    FIND_SUCCESS,
    FIND_FAILURE,
    FIND_ERROR
} FindResult;

typedef struct SearchTarget {
    char* target;
    size_t size;
} SearchTarget;

typedef struct Match {
    size_t start;
    size_t end;
} Match;

typedef struct Finder {
    SearchTarget* target;
    size_t matched;
    size_t currentIndex;
    Match** currentMatches;
    size_t currentMatchCount;
    size_t* prefix;
    size_t fetchIndex;
} Finder;

Finder* createFinder(const char* target, size_t size);

FindResult find(Finder* finder, const char* data, size_t dataSize); // продолжает поиск для новой порции данных

// вернет null если совпадений нет
Match* getMatch(Finder* finder); // достает первое совпадение и удалет его из списка взварщаю копию

void freeMatches(Finder* finder);

void destroyFinder(Finder* finder);
