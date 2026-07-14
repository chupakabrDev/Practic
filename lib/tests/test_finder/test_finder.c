#include <unity.h>
#include <finder.h>
#include <stdlib.h>
#include <string.h>

// --------------------------------------------------------------
// Настройка окружения (Unity)
// --------------------------------------------------------------
void setUp(void) {}
void tearDown(void) {}

// --------------------------------------------------------------
// Вспомогательный макрос для проверки совпадений
// --------------------------------------------------------------
// В структуре Match поле end хранит последний индекс ВКЛЮЧИТЕЛЬНО,
// поэтому длина вычисляется как (end - start + 1).
#define ASSERT_MATCH(m, expected_start, expected_len) \
    do { \
        TEST_ASSERT_NOT_NULL(m); \
        TEST_ASSERT_EQUAL(expected_start, m->start); \
        TEST_ASSERT_EQUAL(expected_len, m->end - m->start + 1); \
    } while(0)

// --------------------------------------------------------------
// 1. Создание и уничтожение Finder
// --------------------------------------------------------------
void test_create_destroy(void) {
    Finder* f = createFinder((const unsigned char*)"abc", 3);
    TEST_ASSERT_NOT_NULL(f);
    destroyFinder(f);
}

// --------------------------------------------------------------
// 2. Одиночное вхождение
// --------------------------------------------------------------
void test_find_single_match(void) {
    Finder* f = createFinder((const unsigned char*)"abc", 3);
    FindResult res = find(f, (const unsigned char*)"abc", 3);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res);

    Match* m = getMatch(f);
    ASSERT_MATCH(m, 0, 3);
    free(m);

    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// 3. Множественные неперекрывающиеся вхождения (одним вызовом find)
// --------------------------------------------------------------
void test_find_multiple_matches(void) {
    Finder* f = createFinder((const unsigned char*)"ab", 2);
    FindResult res = find(f, (const unsigned char*)"ababab", 6);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res);

    Match* m1 = getMatch(f);
    ASSERT_MATCH(m1, 0, 2);
    free(m1);

    Match* m2 = getMatch(f);
    ASSERT_MATCH(m2, 2, 2);
    free(m2);

    Match* m3 = getMatch(f);
    ASSERT_MATCH(m3, 4, 2);
    free(m3);

    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// 4. Обработка перекрывающихся вхождений (ключевой тест!)
// --------------------------------------------------------------
void test_overlap_handling(void) {
    // "aaa" + паттерн "aa" → должно быть только 1 вхождение (не 2)
    Finder* f1 = createFinder((const unsigned char*)"aa", 2);
    FindResult res1 = find(f1, (const unsigned char*)"aaa", 3);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res1);
    Match* m1 = getMatch(f1);
    ASSERT_MATCH(m1, 0, 2);
    free(m1);
    TEST_ASSERT_NULL(getMatch(f1));
    destroyFinder(f1);

    // "aaaaa" + паттерн "aa" → 2 вхождения (0-1 и 2-3, символ 4 остаётся)
    Finder* f2 = createFinder((const unsigned char*)"aa", 2);
    FindResult res2 = find(f2, (const unsigned char*)"aaaaa", 5);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res2);
    Match* m2a = getMatch(f2);
    ASSERT_MATCH(m2a, 0, 2);
    free(m2a);
    Match* m2b = getMatch(f2);
    ASSERT_MATCH(m2b, 2, 2);
    free(m2b);
    TEST_ASSERT_NULL(getMatch(f2));
    destroyFinder(f2);

    // "ababa" + паттерн "aba" → только 1 (0-2), т.к. второе перекрывается
    Finder* f3 = createFinder((const unsigned char*)"aba", 3);
    FindResult res3 = find(f3, (const unsigned char*)"ababa", 5);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res3);
    Match* m3 = getMatch(f3);
    ASSERT_MATCH(m3, 0, 3);
    free(m3);
    TEST_ASSERT_NULL(getMatch(f3));
    destroyFinder(f3);
}

// --------------------------------------------------------------
// 5. Потоковый режим (порционная подача)
// --------------------------------------------------------------
void test_streaming_single_match(void) {
    Finder* f = createFinder((const unsigned char*)"abc", 3);

    FindResult res1 = find(f, (const unsigned char*)"ab", 2);
    TEST_ASSERT_EQUAL(FIND_FAILURE, res1);

    FindResult res2 = find(f, (const unsigned char*)"c", 1);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res2);

    Match* m = getMatch(f);
    ASSERT_MATCH(m, 0, 3);
    free(m);
    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

void test_streaming_multiple_matches(void) {
    Finder* f = createFinder((const unsigned char*)"ab", 2);

    FindResult r1 = find(f, (const unsigned char*)"a", 1); TEST_ASSERT_EQUAL(FIND_FAILURE, r1);
    FindResult r2 = find(f, (const unsigned char*)"b", 1); TEST_ASSERT_EQUAL(FIND_SUCCESS, r2);
    FindResult r3 = find(f, (const unsigned char*)"a", 1); TEST_ASSERT_EQUAL(FIND_FAILURE, r3);
    FindResult r4 = find(f, (const unsigned char*)"b", 1); TEST_ASSERT_EQUAL(FIND_SUCCESS, r4);
    FindResult r5 = find(f, (const unsigned char*)"a", 1); TEST_ASSERT_EQUAL(FIND_FAILURE, r5);
    FindResult r6 = find(f, (const unsigned char*)"b", 1); TEST_ASSERT_EQUAL(FIND_SUCCESS, r6);

    Match* m1 = getMatch(f); ASSERT_MATCH(m1, 0, 2); free(m1);
    Match* m2 = getMatch(f); ASSERT_MATCH(m2, 2, 2); free(m2);
    Match* m3 = getMatch(f); ASSERT_MATCH(m3, 4, 2); free(m3);
    TEST_ASSERT_NULL(getMatch(f));

    destroyFinder(f);
}

// --------------------------------------------------------------
// 6. getMatch удаляет совпадение из списка
// --------------------------------------------------------------
void test_getMatch_removes(void) {
    Finder* f = createFinder((const unsigned char*)"a", 1);
    FindResult res = find(f, (const unsigned char*)"aaa", 3);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res);

    Match* m1 = getMatch(f);
    TEST_ASSERT_NOT_NULL(m1);
    free(m1);

    Match* m2 = getMatch(f);
    TEST_ASSERT_NOT_NULL(m2);
    free(m2);

    Match* m3 = getMatch(f);
    TEST_ASSERT_NOT_NULL(m3);
    free(m3);

    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// 7. Отсутствие совпадений
// --------------------------------------------------------------
void test_no_match(void) {
    Finder* f = createFinder((const unsigned char*)"abc", 3);
    FindResult res = find(f, (const unsigned char*)"def", 3);
    TEST_ASSERT_EQUAL(FIND_FAILURE, res);
    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// 8. freeMatches очищает все накопленные совпадения
// --------------------------------------------------------------
void test_freeMatches_clears(void) {
    Finder* f = createFinder((const unsigned char*)"a", 1);
    FindResult res = find(f, (const unsigned char*)"aaa", 3);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res);
    freeMatches(f);
    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// 9. Длинный паттерн (стресс-тест)
// --------------------------------------------------------------
void test_long_pattern(void) {
    unsigned char target[100] = {0};
    memset(target, 'a', 99);
    Finder* f = createFinder(target, 99);

    unsigned char data[200];
    memset(data, 'a', 200);

    FindResult res = find(f, data, 200);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res);

    Match* m1 = getMatch(f);
    ASSERT_MATCH(m1, 0, 99);
    free(m1);

    Match* m2 = getMatch(f);
    ASSERT_MATCH(m2, 99, 99);
    free(m2);

    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// 10. Несколько совпадений за один вызов find
// --------------------------------------------------------------
void test_multiple_in_one_call(void) {
    Finder* f = createFinder((const unsigned char*)"ab", 2);
    FindResult res = find(f, (const unsigned char*)"abab", 4);
    TEST_ASSERT_EQUAL(FIND_SUCCESS, res);

    Match* m1 = getMatch(f);
    ASSERT_MATCH(m1, 0, 2);
    free(m1);

    Match* m2 = getMatch(f);
    ASSERT_MATCH(m2, 2, 2);
    free(m2);

    TEST_ASSERT_NULL(getMatch(f));
    destroyFinder(f);
}

// --------------------------------------------------------------
// Главная функция – запуск всех тестов
// --------------------------------------------------------------
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_create_destroy);
    RUN_TEST(test_find_single_match);
    RUN_TEST(test_find_multiple_matches);
    RUN_TEST(test_overlap_handling);
    RUN_TEST(test_streaming_single_match);
    RUN_TEST(test_streaming_multiple_matches);
    RUN_TEST(test_getMatch_removes);
    RUN_TEST(test_no_match);
    RUN_TEST(test_freeMatches_clears);
    RUN_TEST(test_long_pattern);
    RUN_TEST(test_multiple_in_one_call);

    return UNITY_END();
}