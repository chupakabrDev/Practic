cmake_minimum_required(VERSION 4.1)
project(lib VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
        src/finder.c
        src/reader.c
        src/writer.c
        src/util.c
        tests/test_finder/test_finder.c
        tests/test_reader/test_reader.c
        tests/test_writer/test_writer.c
)

add_library(lib STATIC ${SOURCES})

target_include_directories(lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
