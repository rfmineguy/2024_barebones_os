#include "memset_t.h"
void*(*memset_kernel)(void*, int, unsigned long) = &memset;

#include <string.h>

MunitResult memset_t(const MunitParameter params[], void* fixture) {
    char buf1[100], buf2[100];
    memset_kernel(buf1, 0, 100);
    memset(buf2, 0, 100);
    munit_assert_memory_equal(100, buf1, buf2);
    return MUNIT_OK;
}
