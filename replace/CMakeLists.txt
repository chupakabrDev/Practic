cmake_minimum_required(VERSION 4.1)
project(replace LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(replace_lib STATIC src/replace.c)
target_link_libraries(replace_lib PUBLIC lib)
target_include_directories(replace_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(replace src/main.c)

target_link_libraries(replace PRIVATE replace_lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
