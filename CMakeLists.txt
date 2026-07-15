cmake_minimum_required(VERSION 4.1)
project(Practic)

set(BUILD_TARGET "none" CACHE STRING "Выбор подпроекта для сборки исполняемого файла")
set_property(CACHE BUILD_TARGET PROPERTY STRINGS "none" "replace" "hexdump" "passgen")

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

if(BUILD_TARGET STREQUAL "replace")
    add_subdirectory(replace)
elseif(BUILD_TARGET STREQUAL "hexdump")
    add_subdirectory(hexdump)
elseif(BUILD_TARGET STREQUAL "passgen")
    add_subdirectory(passgen)
endif()

