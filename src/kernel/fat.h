#ifndef FAT_H
#define FAT_H
#include "../stdlib/stdbool.h"
#include "../stdlib/stdint.h"

typedef struct {
} dir_entry;

typedef struct {
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    // extended boot record
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;          // serial number, value doesn't matter
    uint8_t VolumeLabel[11];    // 11 bytes, padded with spaces
    uint8_t SystemId[8];
} boot_sector;

void       fat_debug();

void       fat_init(uint32_t, uint32_t);

bool       fat_read_header();
bool       fat_read_sectors();
bool       fat_read();

bool       fat_read_root_dir();
dir_entry* fat_find_file(const char* name);
bool       fat_read_file(dir_entry*, uint8_t* outBuff);

#endif
