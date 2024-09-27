#include "munit.h"
#include "printf_t.h"

void* setup(const MunitParameter params[], void* userdata) {
  return NULL;
}

void teardown(void* fixture) {}

MunitTest printf_tests[] = {
    {"/%d", printf_d, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%c", printf_c, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%s", printf_s, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    // {"/%b", printf_b, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%x", printf_x, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%X", printf_X, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    {"/%%", printf_percent, setup, teardown, MUNIT_TEST_OPTION_NONE, NULL},
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

MunitSuite suite = {
    "sprintf", printf_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char** argv) {
    return munit_suite_main(&suite, NULL, argc, argv);
}
