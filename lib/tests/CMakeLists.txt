
# Находим все поддиректории с тестами (test_finder, test_reader, test_writer)
file(GLOB TEST_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} */)

foreach(TEST_DIR ${TEST_DIRS})
    # Убираем завершающий слеш (если есть)
    string(REGEX REPLACE "/$" "" TEST_DIR_CLEAN ${TEST_DIR})

    # Ищем все .c файлы в этой директории (обычно один файл, но можно и больше)
    file(GLOB TEST_SOURCES ${TEST_DIR_CLEAN}/*.c)

    if(TEST_SOURCES)
        # Имя цели = имя директории (например, test_finder)
        set(TARGET_NAME ${TEST_DIR_CLEAN})

        add_executable(${TARGET_NAME} ${TEST_SOURCES})
        target_link_libraries(${TARGET_NAME} PRIVATE lib unity)

        # Регистрируем тест в CTest
        add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
    endif()
endforeach()