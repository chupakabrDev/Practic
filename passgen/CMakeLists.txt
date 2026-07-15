cmake_minimum_required(VERSION 4.1)
project(passgen LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(passgen src/main.c)

target_link_libraries(passgen PUBLIC lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
