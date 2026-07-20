cmake_minimum_required(VERSION 4.1)
project(list LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(list_lib STATIC src/list.c)
target_link_libraries(list_lib PUBLIC lib)
target_include_directories(list_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(list src/main.c)

target_link_libraries(list PUBLIC list_lib)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
