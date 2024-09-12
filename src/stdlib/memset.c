#include "memset.h"

void *memset(void* data, int c, size_t len) {
    for (size_t i = 0; i < len; i++) {
        unsigned char cc = c;
        *(unsigned char*)(data+i) = cc;
    }
    return data;
}
