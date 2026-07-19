/*
Реализация util.h

Кинев Алексей Александрович
МК-102
*/

#include "util.h"

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

unsigned char hexToNibble(const char c) {
    if (c >= '0' && c <= '9') return (unsigned char)(c - '0');
    if (c >= 'A' && c <= 'F') return (unsigned char)(c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (unsigned char)(c - 'a' + 10);

    return 0xFF;
}

unsigned char* hexToBytes(const char* hex, size_t* outLen) {
    if (hex == nullptr) return nullptr;

    size_t hexLen = strlen(hex);
    if (hexLen % 2 != 0 || hexLen == 0) return nullptr;

    *outLen = hexLen /= 2;
    unsigned char* bytes = malloc(hexLen);
    if (bytes == nullptr) return nullptr;

    for (size_t i = 0; i < hexLen; i++) {
        const unsigned char high = hexToNibble(hex[2 * i]);
        const unsigned char low  = hexToNibble(hex[2 * i + 1]);
        if (high == 0xFF || low == 0xFF) {
            free(bytes);
            return nullptr;
        }

        bytes[i] = high << 4 | low;
    }

    return bytes;
}

void byteToHex(const unsigned char b, char *out) {
    static constexpr char hex[] = "0123456789ABCDEF";
    out[0] = hex[b >> 4];
    out[1] = hex[b & 0x0F];
}

size_t strToSizeT(const char *str, char **endptr) {
    errno = 0;
    const uintmax_t val = strtoumax(str, endptr, 10);
    if (errno == ERANGE && val == UINTMAX_MAX)
        return (size_t)-1;
    if (val > SIZE_MAX) {
        errno = ERANGE;
        return (size_t)-1;
    }
    return val;
}

bool startsWith(const char *str, const char *prefix) {
    if (!str || !prefix) return false;

    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool startsWithAny(const char *str, const char *arr, const size_t len) {
    if (!str || !arr) return false;

    for (int i = 0; i < len; i++) {
        if (str[0] == arr[i]) return true;
    }

    return false;
}

void* putError(const char *str, char **error) {
    char *copy = strdup(str);
    if (!copy) {
        perror("Ошибка выделения памяти: putError");
        exit(1);
    }

    *error = copy;
    return nullptr;
}

void* putErrorf(const char *str, char **error, ...) {
    va_list args;
    va_start(args, error);

    va_list argsCopy;
    va_copy(argsCopy, args);
    int len = vsnprintf(nullptr, 0, str, argsCopy);
    va_end(argsCopy);

    if (len < 0) {
        perror("putErrorf: vsnprintf failed");
        exit(1);
    }

    char *copy = malloc(len + 1);
    if (!copy) {
        perror("Ошибка выделения памяти: putErrorf");
        exit(1);
    }

    vsnprintf(copy, len + 1, str, args);
    va_end(args);

    *error = copy;
    return NULL;
}

bool strIsDigit(const char *str) {
    if (str == nullptr) return false;

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i]))
            return false;
    }

    return true;
}

int strtolSafe(const char *nptr, char **endptr, int base) {
    errno = 0;
    long val = strtol(nptr, endptr, base);

    if (errno == ERANGE || val < INT_MIN || val > INT_MAX) {
        errno = ERANGE;
        return (val < 0) ? INT_MIN : INT_MAX;
    }
    return (int)val;
}

void swap(char* a, char* b, const size_t size) {
    for (size_t i = 0; i < size; i++) {
        const char t = a[i];
        a[i] = b[i];
        b[i] = t;
    }
}
