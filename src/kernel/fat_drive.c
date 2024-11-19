#include "fat_drive.h"
#include "ata.h"
#include "log.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/string.h"
#include "../stdlib/stdint.h"
#include "../stdlib/ctype.h"

boot_sector g_boot_sector;
uint8_t*    g_Fat         = (void*)0;
dir_entry*  g_root_directory = (void*)0;
uint32_t    g_root_directory_end;


bool fat_isvalid_filename(const char* filename) {
    int dot_count = 0;
    for (int i = 0; i < strlen(filename); i++) {
        if (filename[i] == '.') {
            dot_count++;
        }
    }

    if (dot_count != 1) return false;
    if (strlen(filename) - 1 > 11) return false;
    return true;
}

bool fat_drive_filename_to_8_3(const char* filename, char* name_8_3) {
    if (!fat_isvalid_filename(filename)) return -1;
    int dot_loc = 0;

    // find dot
    for (int i = 0; i < strlen(filename); i++)
        if (filename[i] == '.') dot_loc = i;

    // copy up to the first 8 chars of filename
    int copy_to = dot_loc > 8 ? 8 : dot_loc;
    for (int i = 0; i < copy_to; i++)
        name_8_3[i] = toupper(filename[i]);

    // copy the first 3 chars of extension
    for (int i = 0; i < 3; i++)
        name_8_3[8 + i] = toupper(filename[dot_loc + i + 1]);
    return 0;
}

void fat_drive_debug_header() {
    log_group_begin("FatDebug");
    log_info("DriveDebug", "Begin---");
    log_info("DriveDebug", "BytesPerSector      =%d", g_boot_sector.BytesPerSector);
    log_info("DriveDebug", "SectorsPerCluster   =%d", g_boot_sector.SectorsPerCluster);
    log_info("DriveDebug", "ReservedSectors     =%d", g_boot_sector.ReservedSectors);
    log_info("DriveDebug", "FatCount            =%d", g_boot_sector.FatCount);
    log_info("DriveDebug", "DirEntryCount       =%d", g_boot_sector.DirEntryCount);
    log_info("DriveDebug", "TotalSectors        =%d", g_boot_sector.TotalSectors);
    log_info("DriveDebug", "MediaDescriptorType =0x%X", g_boot_sector.MediaDescriptorType);
    log_info("DriveDebug", "SectorsPerFat       =0x%X", g_boot_sector.SectorsPerFat);
    log_info("DriveDebug", "SectorsPerTrack     =0x%X", g_boot_sector.SectorsPerTrack);
    log_info("DriveDebug", "Heads               =0x%X", g_boot_sector.Heads);
    log_info("DriveDebug", "HiddenSectors       =0x%X", g_boot_sector.HiddenSectors);
    log_info("DriveDebug", "LargeSectorCount    =0x%X", g_boot_sector.LargeSectorCount);
    log_info("DriveDebug", "End---");
    log_group_end("FatDebug");
}

bool fat_drive_read_header(){
    log_group_begin("ReadHeader");
    uint8_t buf[512];
    ata_read(0xE0, 0, buf, 1);

    g_boot_sector.BytesPerSector = buf[11] | (buf[12] << 8);
    g_boot_sector.SectorsPerCluster = buf[13];
    g_boot_sector.ReservedSectors = buf[14] | (buf[15] << 8);
    g_boot_sector.FatCount = buf[16];
    // NOTE: This masking is kind of hacky for now, without it for some reason i get the wrong%
    //       value
    g_boot_sector.DirEntryCount = (buf[17] | (buf[18] << 8));
    g_boot_sector.TotalSectors = buf[19] | (buf[20] << 8);
    g_boot_sector.MediaDescriptorType = buf[21];
    g_boot_sector.SectorsPerFat = buf[22] | (buf[23] << 8);
    g_boot_sector.SectorsPerTrack = buf[24] | (buf[25] << 8);
    g_boot_sector.Heads = buf[26] | (buf[27] << 8);
    g_boot_sector.HiddenSectors = buf[28] | (buf[29] << 8) | (buf[30] << 16) | (buf[31] << 24);

    log_group_end("ReadHeader");
    return true; // TODO: Add more complex error handling here
}

bool fat_drive_read_sectors(uint32_t lba, uint32_t count, uint8_t* buf_out) {
    ata_read(0xE0, lba, buf_out, count);
    return true; // TODO: Add more complex error handling here
}

bool fat_drive_read(arena* arena){
    log_group_begin("FatDriveRead");
    g_Fat = arena_alloc(arena, g_boot_sector.SectorsPerFat * g_boot_sector.BytesPerSector);
    // bool r = fat_drive_read_sectors(g_boot_sector.ReservedSectors, g_boot_sector.SectorsPerFat, g_Fat);
    ata_read(0xE0, g_boot_sector.ReservedSectors, g_Fat, g_boot_sector.SectorsPerFat);
    log_line_begin("Bytes");
    for (int i = 0; i < sizeof(g_boot_sector); i++) {
        log_line("%x ", ((char*)&g_boot_sector)[i]);
    }
    log_line_end("Bytes");
    log_group_end("FatDriveRead");
    return true;
}

bool fat_drive_read_root_dir(arena* arena){
    log_group_begin("FatDriveReadRootDir");

    // variables
    uint32_t lba = g_boot_sector.ReservedSectors + g_boot_sector.SectorsPerFat * g_boot_sector.FatCount;
    uint32_t size = sizeof(dir_entry) * g_boot_sector.DirEntryCount;
    uint32_t sectors = (size / g_boot_sector.BytesPerSector);
    log_info("ReadRootDir", "lba: %d, size: %d, sectors: %d", lba, size, sectors);
    log_info("ReadRootDir", "BytesPerSector: %d", g_boot_sector.BytesPerSector);

    if (size % g_boot_sector.BytesPerSector > 0)
        sectors++;
    g_root_directory_end = lba + sectors;
    g_root_directory = (dir_entry*) arena_alloc(arena, sectors * g_boot_sector.BytesPerSector);
    // bool r = fat_drive_read_sectors(lba, sectors, g_root_directory);
    ata_read(0xE0, lba, (uint8_t*)g_root_directory, sectors);

    log_line_begin("Bytes");
    ((char*)g_root_directory)[0] = 0x53;
    for (int i = 0; i < g_boot_sector.BytesPerSector; i++) {
        log_line("%X ", ((char*)g_root_directory)[i]);
    }
    log_line_end("Bytes");
    log_group_end("FatDriveReadRootDir");
    return true;
}

dir_entry* fat_drive_find_file(const char* name){
    log_group_begin("FatDriveFindFile");
    log_info("FatFind", "DirEntryCount: %d", g_boot_sector.DirEntryCount);
    for (uint32_t i = 0; i < g_boot_sector.DirEntryCount; i++) {
        if (memcmp(name, g_root_directory[i].Name, 11) == 0) {
            log_group_end("FatDriveFindFile");
            return &g_root_directory[i];
        }
    }
    log_crit("FatFind", "Failed to find '%s'", name);
    log_group_end("FatDriveFindFile");
    return (void*)0;
}

bool fat_drive_read_file(dir_entry* entry, uint8_t* outbuff){
    log_group_begin("FatDriveReadFile");
    bool ok = true;
    uint16_t current_cluster = entry->FirstClusterLow;

    do {
        uint32_t lba = (g_root_directory_end + (current_cluster - 2)) * g_boot_sector.SectorsPerCluster;
        ok = ok && fat_drive_read_sectors(lba, g_boot_sector.SectorsPerCluster, outbuff);
        outbuff += g_boot_sector.SectorsPerCluster * g_boot_sector.BytesPerSector;

        uint32_t fatIndex = current_cluster * 3 / 2;
        if (current_cluster % 2 == 0)
            current_cluster = (*(uint16_t*)(g_Fat + fatIndex)) & 0x0fff;
        else
            current_cluster = (*(uint16_t*)(g_Fat + fatIndex)) >> 4;
    } while (ok && current_cluster < 0x0ff8);
    log_group_end("FatDriveReadFile");
    return ok;
}