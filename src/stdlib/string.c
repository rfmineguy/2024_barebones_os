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

int strcmp(const char* str1, const char* str2) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int minl = len1 < len2 ? len1 : len2;
    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < minl; i++) {
        sum1 += str1[i];
        sum2 += str2[i];
    }
    return sum1 - sum2;
}

int strlen(const char* str) {
    int len = 0;
    while (*str) {
        len ++;
        str ++;
    }
    return len;
}

char *strrchr(const char *s, int c) {
    while (*s != '\0') { // iterate until the end of the string
        if (*s == (char)c) { // check if the current character matches the target
            return (char *)s; // return the address of the last occurrence
        }
        s++; // move to the next character
    }
    return (void*)0; // return NULL if the character is not found
}
