#include <unity.h>
#include <reader.h>
#include <stdio.h>
#include <replace.h>
#include <stdlib.h>

#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

void setUp(void) {
}

void tearDown(void) {
    remove(INPUT_FILE);
    remove(OUTPUT_FILE);
}

static void createInputFile(const char* data, const size_t len) {
    FILE* input = fopen(INPUT_FILE, "wb");
    fwrite(data, 1, len, input);
    fclose(input);
}

static void checkOutput(const void *expected, size_t expectedLen)
{
    FILE *f = fopen(OUTPUT_FILE, "rb");
    TEST_ASSERT_NOT_NULL(f);

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    TEST_ASSERT_EQUAL_UINT32(expectedLen, size);

    unsigned char *buffer = malloc(size);
    TEST_ASSERT_NOT_NULL(buffer);

    fread(buffer, 1, size, f);
    fclose(f);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buffer, expectedLen);

    free(buffer);
}

void testReplaceSingleByte(void)
{
    createInputFile("12345", 5);

    TEST_ASSERT_EQUAL_INT(
        0,
        replace(INPUT_FILE, OUTPUT_FILE,
                "31", "61", 1)
    );

    checkOutput("a2345", 5);
}

void testPatternNotFound(void)
{
    createInputFile("12345", 5);

    TEST_ASSERT_EQUAL_INT(
        0,
        replace(INPUT_FILE, OUTPUT_FILE,
                "39", "61", 1)
    );

    checkOutput("12345", 5);
}

void testReplaceSeveral(void)
{
    createInputFile("11111", 5);

    TEST_ASSERT_EQUAL_INT(
        0,
        replace(INPUT_FILE, OUTPUT_FILE,
                "31", "32", 1)
    );

    checkOutput("22222", 5);
}

void testReplaceTwoBytes(void)
{
    createInputFile("abcabc", 6);

    TEST_ASSERT_EQUAL_INT(
        0,
        replace(INPUT_FILE, OUTPUT_FILE,
                "6162", "7879", 2)
    );

    checkOutput("xycxyc", 6);
}

void testOverlappingPattern(void)
{
    createInputFile("aaaa", 4);

    TEST_ASSERT_EQUAL_INT(
        0,
        replace(INPUT_FILE, OUTPUT_FILE,
                "6161", "6262", 2)
    );

    checkOutput("bbbb", 4);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(testReplaceSingleByte);
    RUN_TEST(testPatternNotFound);
    RUN_TEST(testReplaceSeveral);
    RUN_TEST(testReplaceTwoBytes);
    RUN_TEST(testOverlappingPattern);

    return UNITY_END();
}