#include "ctype_t.h"
#include "../../src/stdlib/ctype.h"
int(*toupper_kernel)(int) = &toupper;

#include <ctype.h>

MunitResult toupper_t(const MunitParameter params[], void* fixture) {
    for (int ch = 'a'; ch < 'z'; ch++) {
        munit_assert_int(toupper_kernel(ch), ==, toupper(ch));
    }
    return MUNIT_OK;
}
