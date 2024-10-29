#include "fat.h"
#include "log.h"
#include "../stdlib/string.h"

uint32_t    g_fat_start_addr, g_fat_end_addr;

boot_sector g_boot_sector;
uint8_t*    g_Fat            = (void*)0;
dir_entry*  g_root_directory = (void*)0;
uint32_t    g_RootDirectory_end;

void fat_debug() {
    char* oem = (char*)g_boot_sector.OemIdentifier;
    log_info("FatRead", "Begin---\n");
    // log_info("FatRead", "OemId=\"%c%c%c%c%c%c%c%c\"\n",
    //         oem[0], oem[1], oem[2], oem[3], oem[4], oem[5], oem[6], oem[7]);
    log_info("FatRead", "BytesPerSector      =0x%X\n", g_boot_sector.BytesPerSector);
    log_info("FatRead", "SectorsPerCluster   =0x%X\n", g_boot_sector.SectorsPerCluster);
    log_info("FatRead", "ReservedSectors     =0x%X\n", g_boot_sector.ReservedSectors);
    log_info("FatRead", "FatCount            =0x%X\n", g_boot_sector.FatCount);
    log_info("FatRead", "DirEntryCount       =0x%X\n", g_boot_sector.DirEntryCount);
    log_info("FatRead", "TotalSectors        =0x%X\n", g_boot_sector.TotalSectors);
    log_info("FatRead", "MediaDescriptorType =0x%X\n", g_boot_sector.MediaDescriptorType);
    log_info("FatRead", "SectorsPerFat       =0x%X\n", g_boot_sector.SectorsPerFat);
    log_info("FatRead", "SectorsPerTrack     =0x%X\n", g_boot_sector.SectorsPerTrack);
    log_info("FatRead", "Heads               =0x%X\n", g_boot_sector.Heads);
    log_info("FatRead", "HiddenSectors       =0x%X\n", g_boot_sector.HiddenSectors);
    log_info("FatRead", "LargeSectorCount    =0x%X\n", g_boot_sector.LargeSectorCount);
    log_info("FatRead", "End---\n");
}

void fat_init(uint32_t start, uint32_t end) {
    g_fat_start_addr = start;
    g_fat_end_addr = end;
}

uint32_t swap32(uint32_t val) {
    return (val << 24) | ((val & 0x00FF0000) >> 8) | ((val & 0x0000FF00) << 8) | (val >> 24);
}

bool fat_read_header() {
    char* fat_ptr = (char*)g_fat_start_addr;

    g_boot_sector.BytesPerSector = fat_ptr[11] | (fat_ptr[12] << 8);
    g_boot_sector.SectorsPerCluster = fat_ptr[13];
    g_boot_sector.ReservedSectors = fat_ptr[14] | (fat_ptr[15] << 8);
    g_boot_sector.FatCount = fat_ptr[16];
    g_boot_sector.DirEntryCount = fat_ptr[17] | (fat_ptr[18] << 8);
    g_boot_sector.TotalSectors = fat_ptr[19] | (fat_ptr[20] << 8);
    g_boot_sector.MediaDescriptorType = fat_ptr[21];
    g_boot_sector.SectorsPerFat = fat_ptr[22] | (fat_ptr[23] << 8);
    g_boot_sector.SectorsPerTrack = fat_ptr[24] | (fat_ptr[25] << 8);
    g_boot_sector.Heads = fat_ptr[26] | (fat_ptr[27] << 8);
    g_boot_sector.HiddenSectors = fat_ptr[28] | (fat_ptr[29] << 8) | (fat_ptr[30] << 16) | (fat_ptr[31] << 24);

    // memcpy(&g_boot_sector, fat_ptr, sizeof(g_boot_sector));
    // char* oem = (char*)g_boot_sector.OemIdentifier;
    return true;
}
bool fat_read_sectors() {

}
bool fat_read() {

}

bool fat_read_root_dir() {

}
dir_entry* fat_find_file(const char* name) {

}
bool fat_read_file(dir_entry* entry, uint8_t* outBuff) {

}
