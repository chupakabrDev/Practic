cmake_minimum_required(VERSION 4.1)
project(Practic)

option(BUILD_REPLACE "Build the replace executable" OFF)
option(BUILD_HEXDUMP "Build the hexdump executable" OFF)

if(BUILD_TESTING)
    include(CTest)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            Unity
            GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
            GIT_TAG        v2.6.1
    )
    FetchContent_MakeAvailable(Unity)
endif()

add_subdirectory(lib)

if(BUILD_REPLACE)
    add_subdirectory(replace)
endif()

if(BUILD_HEXDUMP)
    add_subdirectory(hexdump)
endif()

