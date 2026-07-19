#include "passgen.h"
#include <stdlib.h>
#include <string.h>

int generatePassword(const char *alphabet, const int length, char *out) {
    if (!alphabet || length <= 0 || !out) return -1;

    const int alphaLen = (int)strlen(alphabet);
    if (alphaLen == 0) return -1;

    for (int i = 0; i < length; i++) {
        const int idx = rand() % alphaLen;
        out[i] = alphabet[idx];
    }

    out[length] = '\0';
    return 0;
}

char* buildAlphabetFromSet(const char *set) {
    if (!set) return nullptr;
    size_t totalLen = 0;

    for (const char *p = set; *p; p++) {
        switch (*p) {
            case 'a': totalLen += strlen(LOWER); break;
            case 'A': totalLen += strlen(UPPER); break;
            case 'D': totalLen += strlen(DIGITS); break;
            case 'S': totalLen += strlen(SPECIAL); break;
            default: return nullptr; // недопустимый символ
        }
    }
    if (totalLen == 0) return nullptr;

    char *alphabet = malloc(totalLen + 1);
    if (!alphabet) return nullptr;
    alphabet[0] = '\0';

    for (const char *p = set; *p; p++) {
        const char *src;
        switch (*p) {
            case 'a': src = LOWER; break;
            case 'A': src = UPPER; break;
            case 'D': src = DIGITS; break;
            case 'S': src = SPECIAL; break;
            default: free(alphabet); return nullptr;
        }
        strcat(alphabet, src);
    }

    return alphabet;
}
