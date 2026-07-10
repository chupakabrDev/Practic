#include <unity.h>
#include <writer.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define WRITER_TEST_FILE "test_writer_output.txt"

void setUp(void) {
    // Удаляем файл перед каждым тестом, чтобы начать с чистого листа
    remove(WRITER_TEST_FILE);
}

void tearDown(void) {
    // Удаляем файл после каждого теста
    remove(WRITER_TEST_FILE);
}

// Тест: запись данных и проверка содержимого
void test_writer_create_and_write(void) {
    Writer* w = createWriter(WRITER_TEST_FILE);
    TEST_ASSERT_NOT_NULL(w);

    size_t written;

    written = writeNext(w, "Hello", 5);
    TEST_ASSERT_EQUAL(5, written);

    written = writeNext(w, " World", 6); // пробел перед World
    TEST_ASSERT_EQUAL(6, written);

    destroyWriter(w);

    // Проверяем, что файл содержит ожидаемые данные
    FILE* f = fopen(WRITER_TEST_FILE, "r");
    TEST_ASSERT_NOT_NULL(f);
    char buf[20] = {0};
    size_t n = fread(buf, 1, sizeof(buf) - 1, f);
    TEST_ASSERT_EQUAL(11, n); // "Hello World" – 11 символов
    TEST_ASSERT_EQUAL_STRING("Hello World", buf);
    fclose(f);
}

// Тест: запись нулевой длины (файл должен быть пустым)
void test_writer_write_empty(void) {
    Writer* w = createWriter(WRITER_TEST_FILE);
    TEST_ASSERT_NOT_NULL(w);

    size_t written = writeNext(w, "", 0);
    TEST_ASSERT_EQUAL(0, written);

    destroyWriter(w);

    FILE* f = fopen(WRITER_TEST_FILE, "r");
    TEST_ASSERT_NOT_NULL(f);
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    TEST_ASSERT_EQUAL(0, size);
    fclose(f);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_writer_create_and_write);
    RUN_TEST(test_writer_write_empty);

    return UNITY_END();
}