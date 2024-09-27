#ifndef TEST_PRINTF_H
#define TEST_PRINTF_H
#include "munit.h"

MunitResult printf_c(const MunitParameter params[], void* fixture);
MunitResult printf_d(const MunitParameter params[], void* fixture);
MunitResult printf_x(const MunitParameter params[], void* fixture);
MunitResult printf_X(const MunitParameter params[], void* fixture);
MunitResult printf_s(const MunitParameter params[], void* fixture);
MunitResult printf_percent(const MunitParameter params[], void* fixture);

#endif
