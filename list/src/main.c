#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

int cmpLength(const ListEntry *a, const ListEntry *b) {
    StringListEntry *sa = containerOf(a, StringListEntry, bubble);
    StringListEntry *sb = containerOf(b, StringListEntry, bubble);

    size_t la = strlen(sa->str);
    size_t lb = strlen(sb->str);

    if (la < lb) return -1;
    if (la > lb) return 1;
    return 0;
}

int cmpLex(const ListEntry *a, const ListEntry *b) {
    StringListEntry *sa = containerOf(a, StringListEntry, quick);
    StringListEntry *sb = containerOf(b, StringListEntry, quick);

    return strcmp(sa->str, sb->str);
}

/* Печать одного списка */
static void printList(ListEntry *head, size_t offset) {
    while (head != NULL) {
        StringListEntry *entry = (StringListEntry *)((char *)head - offset);
        printf("%s\n", entry->str);
        head = head->next;
    }
}

static void buildReversedCopy(ListEntry *srcHead, ListEntry **dstHead, const size_t srcOffset, const size_t dstOffset) {
    *dstHead = nullptr;
    if (!srcHead) return;

    ListEntry *tail = srcHead;
    while (tail->next) tail = tail->next;

    ListEntry *cur = tail;
    while (cur) {
        ListEntry *prev = cur->prev;

        // Получаем указатель на объемлющую структуру через srcOffset
        StringListEntry *entry = (StringListEntry *) ((char *) cur - srcOffset);

        // Берём адрес поля, которое будет использоваться в новом списке
        ListEntry *newEntry = (ListEntry *) ((char *) entry + dstOffset);

        // Добавляем в конец нового списка
        listAddTail(dstHead, newEntry);

        cur = prev;
    }
}

int main() {
    const char *strings[] = {
        "banana",
        "apple",
        "pear",
        "orange",
        "grape",
        "kiwi"
    };
    size_t stringCount = sizeof(strings) / sizeof(strings[0]);

    StringListEntry entries[stringCount] = {};

    ListEntry *headOriginal  = nullptr;
    ListEntry *headReversed  = nullptr;
    ListEntry *headBubble    = nullptr;
    ListEntry *headBubbleRev = nullptr;
    ListEntry *headQuick     = nullptr;
    ListEntry *headQuickRev  = nullptr;

    for (size_t i = 0; i < stringCount; i++) {
        entries[i].str = strings[i];

        listAddTail(&headOriginal,  &entries[i].original);
        listAddTail(&headReversed,  &entries[i].reversed);
        listAddTail(&headBubble,    &entries[i].bubble);
        listAddTail(&headQuick,     &entries[i].quick);
    }

    // 1) Разворот списка reversed
    listReverse(&headReversed);

    // 2) Пузырьковая сортировка bubble по длине строк
    bubbleSort(&headBubble, cmpLength);

    // 3) Восстановление bubbleRev как обратного к bubble
    bubbleSort(&headBubbleRev, cmpLength);
    buildReversedCopy(headBubble, &headBubbleRev,
                  offsetof(StringListEntry, bubble),
                  offsetof(StringListEntry, bubbleRev));

    // 4) Быстрая сортировка quick в лексикографическом порядке
    headQuick = quickSort(headQuick, cmpLex);

    // 5) Восстановление quickRev как обратного к quick
    buildReversedCopy(headQuick, &headQuickRev,
                  offsetof(StringListEntry, quick),
                  offsetof(StringListEntry, quickRev));

    /* ---------- ВЫВОД РЕЗУЛЬТАТОВ ---------- */
    printf("Original:\n");
    printList(headOriginal, offsetof(StringListEntry, original));

    printf("Reversed:\n");
    printList(headReversed, offsetof(StringListEntry, reversed));

    printf("Bubble (by length):\n");
    printList(headBubble, offsetof(StringListEntry, bubble));

    printf("Bubble reversed:\n");
    printList(headBubbleRev, offsetof(StringListEntry, bubbleRev));

    printf("Quick (lexicographic):\n");
    printList(headQuick, offsetof(StringListEntry, quick));

    printf("Quick reversed:\n");
    printList(headQuickRev, offsetof(StringListEntry, quickRev));

    return 0;
}