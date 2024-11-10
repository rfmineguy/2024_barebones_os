#include "fat.h"
#include "log.h"
#include "../stdlib/string.h"
#include "../stdlib/ctype.h"

uint32_t    g_fat_start_addr, g_fat_end_addr;

boot_sector g_boot_sector;
uint8_t*    g_Fat            = (void*)0;
dir_entry*  g_root_directory = (void*)0;
uint32_t    g_root_directory_end;

arena*      karena;

void fat_debug() {
    log_info("FatRead", "Begin---\n");
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

// 8.3 format
//
//
// ________===
//
// _ -> filename
// _ -> extension
int fat_filename_to_8_3(const char* filename, char* name_8_3) {
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

void fat_init(uint32_t fat_start, uint32_t fat_end, arena* arena) {
    g_fat_start_addr = fat_start;
    g_fat_end_addr = fat_end;
    karena = arena;
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

    // NOTE: This masking is kind of hacky for now, without it for some reason i get the wrong%
    //       value
    g_boot_sector.DirEntryCount = (fat_ptr[17] | (fat_ptr[18] << 8)) & 0xff;
    g_boot_sector.TotalSectors = fat_ptr[19] | (fat_ptr[20] << 8);
    g_boot_sector.MediaDescriptorType = fat_ptr[21];
    g_boot_sector.SectorsPerFat = fat_ptr[22] | (fat_ptr[23] << 8);
    g_boot_sector.SectorsPerTrack = fat_ptr[24] | (fat_ptr[25] << 8);
    g_boot_sector.Heads = fat_ptr[26] | (fat_ptr[27] << 8);
    g_boot_sector.HiddenSectors = fat_ptr[28] | (fat_ptr[29] << 8) | (fat_ptr[30] << 16) | (fat_ptr[31] << 24);

    return true;
}

bool fat_read_sectors(uint32_t lba, uint32_t count, void* bufferOut) {
    char* fat_ptr = (char*)g_fat_start_addr;
    int offset = lba * g_boot_sector.BytesPerSector;
    if (offset + (count * g_boot_sector.BytesPerSector) > g_fat_end_addr) {
        return false;
    }
    memcpy(bufferOut, (char*)fat_ptr + offset, count * g_boot_sector.BytesPerSector);
    log_info("ReadSectors", "lba: %x, BytesPerSector: %x\n", lba, g_boot_sector.BytesPerSector);
    log_info("ReadSectors", "offset: %x\n", (uint32_t)offset);
    log_info("ReadSectors", "Copy {from: %x, to: %x}\n", (uint32_t)(char*)(fat_ptr + offset), (uint32_t)bufferOut);
    return true;
}

bool fat_read() {
    g_Fat = arena_alloc(karena, g_boot_sector.SectorsPerFat * g_boot_sector.BytesPerSector);
    log_info("FatRead", "Allocating %d bytes, at %x\n", g_boot_sector.SectorsPerFat * g_boot_sector.BytesPerSector, &g_Fat);
    return fat_read_sectors(g_boot_sector.ReservedSectors, g_boot_sector.SectorsPerFat, g_Fat);
}

bool fat_read_root_dir() {
    uint32_t lba = g_boot_sector.ReservedSectors + g_boot_sector.SectorsPerFat * g_boot_sector.FatCount;
    uint32_t size = sizeof(dir_entry) * g_boot_sector.DirEntryCount;
    uint32_t sectors = (size / g_boot_sector.BytesPerSector);
    if (size % g_boot_sector.BytesPerSector > 0)
        sectors++;

    log_info("FatReadRoot", "lba: %d\n", lba);
    log_info("FatReadRoot", "size: %d\n", size);
    log_info("FatReadRoot", "sectors: %d\n", sectors);
    g_root_directory_end = lba + sectors;
    log_info("FatReadRoot", "Allocating %d bytes\n", sectors * g_boot_sector.BytesPerSector);
    g_root_directory = (dir_entry*) arena_alloc(karena, sectors * g_boot_sector.BytesPerSector);
    return fat_read_sectors(lba, sectors, g_root_directory);
}

dir_entry* fat_find_file(const char* name) {
    for (uint32_t i = 0; i < g_boot_sector.DirEntryCount; i++) {
        if (memcmp(name, g_root_directory[i].Name, 11) == 0)
            return &g_root_directory[i];
    }

    return (void*)0;
}

bool fat_read_file(dir_entry* entry, uint8_t* outBuff) {
    bool ok = true;
    uint16_t current_cluster = entry->FirstClusterLow;

    do {
        uint32_t lba = (g_root_directory_end + (current_cluster - 2)) * g_boot_sector.SectorsPerCluster;
        ok = ok && fat_read_sectors(lba, g_boot_sector.SectorsPerCluster, outBuff);
        outBuff += g_boot_sector.SectorsPerCluster * g_boot_sector.BytesPerSector;

        uint32_t fatIndex = current_cluster * 3 / 2;
        if (current_cluster % 2== 0)
            current_cluster = (*(uint16_t*)(g_Fat + fatIndex)) & 0x0fff;
        else
            current_cluster = (*(uint16_t*)(g_Fat + fatIndex)) >> 4;
    } while (ok && current_cluster < 0x0ff8);

    return ok;
}

uint8_t* fat_read_entry(dir_entry* entry, arena* a) {
    if (entry == (void*)0) return (void*)0;
    uint8_t* buf = arena_alloc(a, entry->Size + g_boot_sector.BytesPerSector);
    if (!fat_read_file(entry, buf)) {
        // Note: Buf is now a dangling pointer...
        //       Not sure how to deal with that
        //       No notion of freeing a single allocation in arena
        //       Though this theoretically should never happen due to the null check of the entry
        return (void*)0;
    }
    return buf;
}
