cmake_minimum_required(VERSION 4.1)
project(hexdump LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(hexdump_lib STATIC src/hexdump.c)
target_link_libraries(hexdump_lib PUBLIC lib)
target_include_directories(hexdump_lib
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_executable(hexdump src/main.c)

target_link_libraries(hexdump PRIVATE hexdump_lib)
