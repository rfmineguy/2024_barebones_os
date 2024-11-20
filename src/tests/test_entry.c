#include "drive_test.h"

test ata_tests[9] = {
    TEST("read", test_ata, 0, 0),
    END()
};

test_group main_group = {
    .name = "ATA Tests",
    .tests = ata_tests
};

int test_entry() {
    return test_group_main(main_group);
}
