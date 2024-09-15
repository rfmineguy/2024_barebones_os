#include "memset.h"

void *memset(void* dest, char c, int count) {
    char *temp = (char*) dest;
    for (; count >= 0; count--) {
        *temp++ = c;
    }
    return dest;
}
