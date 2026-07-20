#include "list.h"

#include <stddef.h>

void listAddTail(ListEntry **head, ListEntry *newEntry) {
    newEntry->next = nullptr;
    if (*head == NULL) {
        *head = newEntry;
        newEntry->prev = nullptr;
    } else {
        ListEntry *cur = *head;
        while (cur->next) cur = cur->next;
        cur->next = newEntry;
        newEntry->prev = cur;
    }
}

void listRemove(ListEntry **head, ListEntry *entry) {
    if (!head || !entry) return;

    if (entry->prev) entry->prev->next = entry->next;
    if (entry->next) entry->next->prev = entry->prev;
    if (*head == entry) *head = entry->next;

    entry->next = entry->prev = nullptr;
}

void listReverse(ListEntry **head) {
    if (*head == NULL) return;

    ListEntry *cur = *head;
    ListEntry *temp = nullptr;

    while (cur) {
        temp = cur->prev;
        cur->prev = cur->next;
        cur->next = temp;
        cur = cur->prev;
    }

    if (temp)
        *head = temp->prev;
}

void swapEntries(ListEntry *a, ListEntry *b) {
    if (a == b) return;

    // Случай 1: a и b — соседи, a сразу перед b
    if (a->next == b) {
        ListEntry *aPrev = a->prev;
        ListEntry *bNext = b->next;

        // Перелинковка
        if (aPrev) aPrev->next = b;
        b->prev = aPrev;
        b->next = a;
        a->prev = b;
        a->next = bNext;
        if (bNext) bNext->prev = a;
    }
    // Случай 2: a и b — соседи, b сразу перед a
    else if (b->next == a) {
        ListEntry *bPrev = b->prev;
        ListEntry *aNext = a->next;

        if (bPrev) bPrev->next = a;
        a->prev = bPrev;
        a->next = b;
        b->prev = a;
        b->next = aNext;
        if (aNext) aNext->prev = b;
    }
    // Случай 3: узлы не соседние
    else {
        ListEntry *aPrev = a->prev;
        ListEntry *aNext = a->next;
        ListEntry *bPrev = b->prev;
        ListEntry *bNext = b->next;

        // Предыдущие узлы теперь указывают на "чужие" узлы
        if (aPrev) aPrev->next = b;
        if (bPrev) bPrev->next = a;

        // Меняем ссылки у самих узлов
        b->prev = aPrev;
        b->next = aNext;
        a->prev = bPrev;
        a->next = bNext;

        // Следующие узлы теперь указывают назад на "чужие" узлы
        if (aNext) aNext->prev = b;
        if (bNext) bNext->prev = a;
    }
}

void bubbleSort(ListEntry **head, int (*cmp)(const ListEntry*, const ListEntry*)) {
    if (!head || !*head) return;

    int swapped;
    do {
        swapped = 0;
        ListEntry *cur = *head;
        while (cur->next != NULL) {
            if (cmp(cur, cur->next) > 0) {
                ListEntry *a = cur;
                ListEntry *b = cur->next;
                swapEntries(a, b);

                if (*head == a) *head = b;
                else if (*head == b) *head = a;

                swapped = 1;
                break;
            }
            cur = cur->next;
        }
    } while (swapped);
}

ListEntry* quickSort(ListEntry *head, int (*cmp)(const ListEntry*, const ListEntry*)) {
    if (!head || !head->next) return head;

    // Выбираем опорный – первый элемент
    ListEntry *pivot = head;
    // Удаляем pivot из списка
    head = head->next;
    if (head) head->prev = nullptr;
    pivot->next = pivot->prev = nullptr;

    // Создаём три пустых списка
    ListEntry *less = nullptr, *equal = nullptr, *greater = nullptr;
    equal = pivot;

    // Проходим по оставшимся узлам и распределяем
    ListEntry *cur = head;
    while (cur) {
        ListEntry *next = cur->next;
        // Отцепляем cur от списка
        listRemove(&head, cur);
        // Сравниваем с pivot
        int res = cmp(cur, pivot);
        if (res < 0) {
            listAddTail(&less, cur);
        } else if (res > 0) {
            listAddTail(&greater, cur);
        } else {
            listAddTail(&equal, cur);
        }
        cur = next;
    }

    // Рекурсивно сортируем less и greater
    less = quickSort(less, cmp);
    greater = quickSort(greater, cmp);

    // Объединяем: less + equal + greater
    ListEntry *result = less;
    if (result) {
        ListEntry *tail = result;
        while (tail->next) tail = tail->next;
        tail->next = equal;
        equal->prev = tail;
    } else {
        result = equal;
    }

    // Находим хвост equal
    ListEntry *tail = equal;
    while (tail->next) tail = tail->next;
    tail->next = greater;
    if (greater) greater->prev = tail;

    return result;
}
