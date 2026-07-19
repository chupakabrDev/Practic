cmake_minimum_required(VERSION 4.1)
project(passgen LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(passgen_lib STATIC src/passgen.c)
target_link_libraries(passgen_lib PUBLIC lib)
target_include_directories(passgen_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(passgen src/main.c)

target_link_libraries(passgen PUBLIC passgen_lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
