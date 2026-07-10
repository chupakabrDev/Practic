#include <unity.h>
#include <reader.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define READER_TEST_FILE "test_reader_input.txt"
#define READ_SIZE 5

void setUp(void) {
    // Создаём файл с тестовыми данными перед каждым тестом
    FILE* f = fopen(READER_TEST_FILE, "w");
    TEST_ASSERT_NOT_NULL(f);
    fwrite("Hello World!", 1, 12, f);
    fclose(f);
}

void tearDown(void) {
    // Удаляем файл после каждого теста
    remove(READER_TEST_FILE);
}

// Тест: успешное создание Reader
void test_reader_create_opens_file(void) {
    Reader* r = createReader(READER_TEST_FILE, READ_SIZE);
    TEST_ASSERT_NOT_NULL(r);
    destroyReader(r);
}

// Тест: попытка открыть несуществующий файл
void test_reader_create_fails_for_nonexistent(void) {
    Reader* r = createReader("nonexistent_file_12345.txt", READ_SIZE);
    TEST_ASSERT_NULL(r);
}

// Тест: чтение файла блоками заданного размера
void test_reader_reads_blocks(void) {
    Reader* r = createReader(READER_TEST_FILE, READ_SIZE);
    TEST_ASSERT_NOT_NULL(r);

    size_t n;

    n = readNext(r);
    TEST_ASSERT_EQUAL(5, n);
    TEST_ASSERT_EQUAL_STRING_LEN("Hello", r->buffer, 5);

    n = readNext(r);
    TEST_ASSERT_EQUAL(5, n);
    TEST_ASSERT_EQUAL_STRING_LEN(" Worl", r->buffer, 5);

    n = readNext(r);
    TEST_ASSERT_EQUAL(2, n);
    TEST_ASSERT_EQUAL_STRING_LEN("d!", r->buffer, 2);

    n = readNext(r);
    TEST_ASSERT_EQUAL(0, n);

    destroyReader(r);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_reader_create_opens_file);
    RUN_TEST(test_reader_create_fails_for_nonexistent);
    RUN_TEST(test_reader_reads_blocks);

    return UNITY_END();
}