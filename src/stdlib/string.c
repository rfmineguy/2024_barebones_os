#include "string.h"

void* memcpy(void* dest, const void* src, int n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    for (int i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}
