cmake_minimum_required(VERSION 4.1)
project(readblock LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(readblock_lib STATIC src/readblock.c)
target_link_libraries(readblock_lib PUBLIC lib)
target_include_directories(readblock_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(readblock src/main.c)

target_link_libraries(readblock PUBLIC readblock_lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
