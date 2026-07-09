#include "util.h"
#include <stdio.h>
#include <stdlib.h>

void exitIfNull(const void* ptr, const char* msg) {
    if (ptr == nullptr) {
        fputs(msg, stderr);
        exit(1);
    }
}

void checkAllocateMem(const void* ptr) {
    exitIfNull(ptr, "Ошибка выделения памяти");
}

void checkOpenFile(const void* ptr) {
    exitIfNull(ptr, "Ошибка открытия файла");
}
