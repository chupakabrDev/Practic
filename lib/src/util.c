/*
Реализация util.h

Кинев Алексей Александрович
МК-102
*/

#include "util.h"

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>

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
