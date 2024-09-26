#include <printf.h>
#include "../../src/stdlib/printf.h"
#include "printf_t.h"
#include <limits.h>

MunitResult printf_d(const MunitParameter params[], void* fixture) {
    char buf[100], buf2[100];
    for (unsigned int i = 0; i < UINT_MAX/0xff; i+=0xff) {
        k_sprintf(buf, "%d", i);
        sprintf(buf2, "%u", i);
        munit_assert_string_equal(buf, buf2);
    }
    return MUNIT_OK;
}
