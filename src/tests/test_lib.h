#ifndef TEST_LIB_H
#define TEST_LIB_H
#include "../kernel/log.h"
#include "../stdlib/stdbool.h"

// int test(const char* name, int(*func)(), void(*setup)(), void(*teardown)());

typedef struct test_result {
    int code;
    const char *msg;
} test_result;

typedef struct test {
    const char* name;
    test_result(*test_callback)();
    int(*setup_callback)();
    int(*teardown_callback)();
    bool is_end;
} test;

typedef struct test_group {
    const char* name;
    test* tests;
} test_group;

int test_group_main(test_group);

#define END()\
    (test) {\
        .name="",\
        .test_callback=(void*)0,\
        .setup_callback=(void*)0,\
        .teardown_callback=(void*)0,\
        .is_end=true\
    }

#define TEST(name_, func_, setup_, teardown_)\
    (test) {\
        .name=name_,\
        .test_callback=func_,\
        .setup_callback=setup_,\
        .teardown_callback=teardown_,\
        .is_end=false\
    }

#define STR(x) #x

#define assert_int(a, op, b) \
    if (!(a op b)) {\
        return (test_result) {.code = -1, .msg = STR((a) ## op ## b)};\
    }\

#define OK() \
    (test_result) {.code = 0, .msg = ""}

#endif
