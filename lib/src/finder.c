#include "finder.h"

#include <stdlib.h>
#include <string.h>

#include "util.h"

static size_t kmpStep(const char* pattern, const size_t* pi, size_t matched, const char c) {
    while (matched > 0 && c != pattern[matched]) {
        matched = pi[matched - 1];
    }

    if (c == pattern[matched]) {
        ++matched;
    }

    return matched;
}

static void computePrefixFunction(const char* pattern, const size_t len, size_t* pi) {
    pi[0] = 0;
    size_t matched = 0;

    for (size_t i = 1; i < len; i++) {
        matched = kmpStep(pattern, pi, matched, pattern[i]);
        pi[i] = matched;
    }

}

Finder* createFinder(const char *target, const size_t size) {
    if (target == nullptr || size == 0)
        return nullptr;

    Finder *finder = malloc(sizeof(Finder));
    checkAllocateMem(finder);

    SearchTarget *search = malloc(sizeof(SearchTarget));
    checkAllocateMem(search);
    search->target = strdup(target);
    checkAllocateMem(search->target);
    search->size = size;

    finder->target = search;

    finder->matched = 0;
    finder->currentIndex = 0;
    finder->currentMatches = nullptr;
    finder->currentMatchCount = 0;
    finder->fetchIndex = 0;

    finder->prefix = calloc(size, sizeof(size));
    checkAllocateMem(finder->prefix);
    computePrefixFunction(search->target, size, finder->prefix);

    return finder;
}

bool find(Finder *finder, const char *data, const size_t dataSize) {
    if (finder == nullptr || data == nullptr || dataSize == 0 || finder->target->size == 0)
        return false;

    const size_t pattern_len = finder->target->size;
    const char* pattern = finder->target->target;
    const size_t* pi = finder->prefix;

    size_t matched = finder->matched;

    const size_t oldCount = finder->currentMatchCount;
    size_t insertIndex = oldCount;

    for (size_t i = 0; i < dataSize; i++) {
        const char c = data[i];
        matched = kmpStep(pattern, pi, matched, c);

        if (matched == pattern_len) {
            const size_t start = finder->currentIndex + i - pattern_len + 1;
            const size_t end = start + pattern_len - 1;

            Match* match = malloc(sizeof(Match));
            checkAllocateMem(match);
            match->start = start;
            match->end = end;

            finder->currentMatchCount++;
            Match** newMatches = realloc(finder->currentMatches, finder->currentMatchCount * sizeof(Match*));
            checkAllocateMem(newMatches);
            finder->currentMatches = newMatches;
            finder->currentMatches[insertIndex++] = match;

            matched = 0;
        }
    }

    finder->matched = matched;
    finder->currentIndex += dataSize;

    return finder->currentMatchCount > oldCount;
}

Match* getMatch(Finder *finder) {
    if (finder == nullptr || finder->fetchIndex >= finder->currentMatchCount || finder->currentMatches == nullptr)
        return nullptr;

    Match* orig = finder->currentMatches[finder->fetchIndex];
    Match* copy = malloc(sizeof(Match));
    checkAllocateMem(copy);
    *copy = *orig;

    free(orig);
    finder->currentMatches[finder->fetchIndex++] = nullptr;

    return copy; // вызывающий должен вызвать free(copy)
}

void freeMatches(Finder* finder) {
    if (finder == nullptr || finder->currentMatches == nullptr) return;

    for (size_t i = 0; i < finder->currentMatchCount; i++) {
        free(finder->currentMatches[i]);
    }

    free(finder->currentMatches);
    finder->currentMatches = nullptr;
}

void destroyFinder(Finder *finder) {
    if (finder == nullptr) return;

    free(finder->prefix);
    finder->prefix = nullptr;

    freeMatches(finder);

    free(finder->target->target);
    finder->target->target = nullptr;

    free(finder->target);
    finder->target = nullptr;

    free(finder);
}
