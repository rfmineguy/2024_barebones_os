#include "files.h"
#include "fat_drive.h"

void files_populate(ui_box_t* box) {
    const dir_entry* root_dir = fat_drive_internal_get_root_dir();
    char filename[11];
    for (uint32_t i = 1; i < fat_drive_internal_get_boot_sector().DirEntryCount; i++) {
        if (root_dir[i].Name[0] == 0x00) break;

        fat_drive_8_3_to_filename((const char*)root_dir[i].Name, filename);
        ui_putstr(box, 1, i - 1, filename);
    }

    // ui_putstr(box, 1, 0, "a.txt");
    // ui_putstr(box, 1, 1, "test.txt");
}
