#include <printf.h>
#include "../../src/stdlib/printf.h"
#include "printf_t.h"
#include <limits.h>

MunitResult printf_c(const MunitParameter params[], void* fixture) {
    char buf[100], buf2[100];
    for (int i = 0x20; i < 0x7e; i++) {
        if (i == '%') continue;
        k_sprintf(buf, "%c", i);
        sprintf(buf2, "%c", i);
        munit_assert_string_equal(buf, buf2);
    }
    return MUNIT_OK;
}

MunitResult printf_d(const MunitParameter params[], void* fixture) {
    char buf[100], buf2[100];
    for (unsigned int i = 0; i < UINT_MAX/0xff; i+=0xff) {
        k_sprintf(buf, "%d", i);
        sprintf(buf2, "%u", i);
        munit_assert_string_equal(buf, buf2);
    }
    return MUNIT_OK;
}

MunitResult printf_x(const MunitParameter params[], void* fixture) {
    char buf[100], buf2[100];
    for (unsigned int i = 0; i < UINT_MAX/0xff; i+=0xff) {
        k_sprintf(buf, "%x", i);
        sprintf(buf2, "%x", i);
        munit_assert_string_equal(buf, buf2);
    }
    return MUNIT_OK;
}
MunitResult printf_X(const MunitParameter params[], void* fixture) {
    char buf[100], buf2[100];
    for (unsigned int i = 0; i < UINT_MAX/0xff; i+=0xff) {
        k_sprintf(buf, "%X", i);
        sprintf(buf2, "%X", i);
        munit_assert_string_equal(buf, buf2);
    }
    return MUNIT_OK;
}
MunitResult printf_s(const MunitParameter params[], void* fixture) {
    char buf0[100];
    char buf1[100], buf2[100];
    for (unsigned int i = 0; i < UINT_MAX/0xff; i+=0xff) {
        // uses known working %d to test %s
        sprintf  (buf0, "%d", i);

        // actual test
        k_sprintf(buf1, "%s", buf0);
        sprintf  (buf2, "%s", buf0);
        munit_assert_string_equal(buf1, buf2);
    }

    return MUNIT_OK;
}
MunitResult printf_percent(const MunitParameter params[], void* fixture) {
    char buf[100], buf2[100];
    k_sprintf(buf, "%%");
    sprintf(buf2, "%%");
    munit_assert_string_equal(buf, buf2);
    return MUNIT_OK;
}
