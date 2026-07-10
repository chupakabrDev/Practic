#include <unity.h>

void setUp(void) {}
void tearDown(void) {}

void test_lib_function(void) {
    int result = some_function_from_lib(5);
    TEST_ASSERT_EQUAL_INT(10, result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_lib_function);
    // добавьте другие тесты
    return UNITY_END();
}