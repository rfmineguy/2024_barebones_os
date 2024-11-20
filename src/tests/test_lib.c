#include "test_lib.h"

int test_group_main(test_group group) {
    test *current_test = group.tests;
    int status = 0;

    log_group_begin("TestGroupMain [%s]", group.name);
    while (current_test != 0 && !current_test->is_end) {
        log_group_begin("Test %s", current_test->name);
        test_result result = current_test->test_callback();
        if (result.code != 0) {
            log_info("Status", "Failed [%s]", result.msg);
        }
        else {
            log_info("Status", "Success");
        }
        log_group_end("Test %s", current_test->name);
        current_test++;
    }
    log_group_end("TestGroupMain [%s]", group.name);

    return status;
}
