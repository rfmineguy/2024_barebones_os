#ifndef FAT_DRIVE_H
#define FAT_DRIVE_H
#include "../stdlib/stdint.h"
#include "../stdlib/stdbool.h"
#include "arena.h"

typedef struct dir_entry {
    uint8_t Name[11];
    uint8_t Attributes;
    uint8_t _Reserved;
    uint8_t CreatedTimeTenths;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint16_t AccessedDate;
    uint16_t FirstClusterHigh;
    uint16_t ModifiedTime;
    uint16_t ModifiedDate;
    uint16_t FirstClusterLow;
    uint32_t Size;
} dir_entry;

typedef struct boot_sector {
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

bool fat_drive_filename_to_8_3(const char*, char*);

/* Desc:   Prints out the boot sector data entries in human readable form
 *         Must be called after 'fat_drive_read_header()' for accurate results
 * Params: N/A
 * Return: N/A
 */
void fat_drive_debug_header();

/* Desc: read the MBR from the active drive (0xE0 for primary)
 *     initializes internal state to reduce complexity (for now)
 * Params: N/A
 * Return: whether the header was successfully read and is a valid fat12
 *     header
 */
bool fat_drive_read_header();

/* Desc:     read sectors from the active drive (0xE0 for primary)
 * Params:   3
 *  lba:     the sector to start reading from
 *  count:   the number of sectors to read
 *  buf_out: pointer to memory to store the read data
 * Return:   whether the read from disk was successful 
 */
bool fat_drive_read_sectors(uint32_t lba, uint32_t count, uint8_t* buf_out);

// WIP
bool fat_drive_read(arena*);

/* Desc:     read the root directory of the fat
 */
bool fat_drive_read_root_dir(arena*);

/* Desc:     search for the dir where the file resides
 * Params:   1
 *  name:    the name of the file to search for (must comply with 8.3 format)
 * Return:   if the search was successful
 *              a valid dir_entry
 *           else
 *              (void*)0
 */
dir_entry* fat_drive_find_file(const char* name);

/* Desc:     read the data from the file pointed to by @entry
 * Params:   2
 *  entry:   pointer to a valid dir_entry obtained via fat_drive_file_file()
 *  outbuff: a pointer to a valid block of memory to store the contents of the file
 * Return:   Whether the read was sucessful
 */
bool fat_drive_read_file(dir_entry* entry, uint8_t* outbuff);

#endif
