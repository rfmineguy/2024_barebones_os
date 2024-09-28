#include "munit.h"
#include "printf_t.h"
#include "memset_t.h"
#include "ctype_t.h"

void* setup(const MunitParameter params[], void* userdata) {
  return NULL;
}

void teardown(void* fixture) {}

MunitTest printf_tests[] = {
    {"/%d", printf_d, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%c", printf_c, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%s", printf_s, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%x", printf_x, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%X", printf_X, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%%", printf_percent, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

MunitTest memset_tests[] = {
    {"/memset", memset_t, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

MunitTest ctype_tests[] = {
    {"/toupper", toupper_t, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

MunitSuite suites[] = {
    { "/sprintf", printf_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/string",  memset_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { "/ctype",   ctype_tests,  NULL, 1, MUNIT_SUITE_OPTION_NONE },
    { NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE }
};

MunitSuite suite = {
    "tests", NULL, suites, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char** argv) {
    return munit_suite_main(&suite, NULL, argc, argv);
}
