cmake_minimum_required(VERSION 4.1)
project(hexdump LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(SOURCES
        src/main.c
)

add_executable(hexdump ${SOURCES})

target_link_libraries(hexdump PRIVATE lib)

