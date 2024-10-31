#include "string.h"

void* memcpy(void* dest, const void* src, int n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;

    for (int i = 0; i < n; i++) {
        d[i] = s[i];
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, int n) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    for (int i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i]; // Return the difference
        }
    }
    return 0; // Blocks are equal
}
