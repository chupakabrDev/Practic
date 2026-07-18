/*
Реализация util.h

Кинев Алексей Александрович
МК-102
*/

#include "util.h"

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
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

void putError(const char *str, char **error) {
    char *copy = strdup(str);
    if (!copy) {
        perror("Ошибка выделения памяти: putError");
        exit(1);
    }

    *error = copy;
}
