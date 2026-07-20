#pragma once

#include <stddef.h>

typedef struct ListEntry {
    struct ListEntry *next;
    struct ListEntry *prev;
} ListEntry;

typedef struct StringListEntry {
    ListEntry original;
    ListEntry reversed;
    ListEntry bubble;
    ListEntry bubbleRev;
    ListEntry quick;
    ListEntry quickRev;

    const char *str;
} StringListEntry;

#define containerOf(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

void listAddTail(ListEntry **head, ListEntry *newEntry);

void listReverse(ListEntry **head);

void swapEntries(ListEntry *a, ListEntry *b);

void bubbleSort(ListEntry **head, int (*cmp)(const ListEntry*, const ListEntry*));

ListEntry* quickSort(ListEntry *head, int (*cmp)(const ListEntry*, const ListEntry*));
