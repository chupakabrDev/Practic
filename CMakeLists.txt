cmake_minimum_required(VERSION 4.1)
project(Practic)

option(BUILD_REPLACE "Build the replace executable" ON)

if(BUILD_REPLACE)
    add_subdirectory(replace)
endif()

add_subdirectory(lib)

