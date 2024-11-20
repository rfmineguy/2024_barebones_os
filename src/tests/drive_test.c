#include "drive_test.h"

    // Test ATA
    // log_group_begin("Drive Test With ATA Directly");
    // uint8_t buf[512 * 14];
    // ata_read(0xE0, 19, buf, 14);
    // log_line_begin("Bytes");
    // for (int i = 0; i < 512; i++) {
    //     log_line("%x ", buf[i]);
    // }
    // log_line_end("Bytes");
    // log_group_end("Drive Test With ATA Directly");

    // log_group_begin("Drive Test With fat_drive.c");
    // fat_drive_read_sectors(19, 14, buf);
    // log_line_begin("Bytes");
    // for (int i = 0; i < 512; i++) {
    //     log_line("%x ", buf[i]);
    // }
    // log_line_end("Bytes");
    // log_group_end("Drive Test With fat_drive.c");

    // log_group_begin("Read a.txt");
    // dir_entry* dir = fat_drive_find_file("A       TXT");
    // if (dir) {
    //     uint8_t* buf = arena_alloc(&kernel_arena, dir->Size);
    //     fat_drive_read_file(dir, buf);
    //     log_info("a.txt", "%s\n", buf);
    // }
    // log_group_end("Read a.txt");

test_result test_ata() {
    return OK();
}
