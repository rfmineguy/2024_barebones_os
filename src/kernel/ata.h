#ifndef ATA_H
#define ATA_H
#include "../stdlib/stdint.h"

#define ATA_STATUS 0x1f7
#define ATA_DATA   0x1f0

#define ATA_DRIVE_HEAD 0x1F6

void ata_wait_ready();
void ata_select_drive(uint8_t drive);
void ata_read(int lba, uint8_t* buffer, uint32_t sectors);

#endif
