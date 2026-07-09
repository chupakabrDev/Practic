cmake_minimum_required(VERSION 4.1)
project(Practic C)

set(CMAKE_C_STANDARD 23)

add_executable(Practic main.c
        reader.h
        reader.c
        util.h
        util.c
        finder.h
        finder.c
        test.c
        test.h
        writer.h
        writer.c)
