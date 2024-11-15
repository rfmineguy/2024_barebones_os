#include "ata.h"
#include "io.h"

void ata_wait_ready(){
    while (io_inb(ATA_STATUS) & 0x80) {}
}
void ata_select_drive(uint8_t drive) {
    io_outb(ATA_DRIVE_HEAD, drive);
    io_wait();
}
void ata_read(int lba, uint8_t* buffer, uint32_t sectors){
    // Wait for the drive to be ready
    ata_wait_ready();

    // Select the drive (e.g., Primary Master)
    ata_select_drive(0xE0);  // Primary Master (or choose another drive)

    // Set the number of sectors to read
    io_outb(0x1F2, sectors); io_wait();

    // Set the LBA address
    io_outb(0x1F3, lba & 0xFF);         io_wait();  // LBA Low
    io_outb(0x1F4, (lba >> 8) & 0xFF);  io_wait();  // LBA Mid
    io_outb(0x1F5, (lba >> 16) & 0xFF); io_wait();  // LBA High

    // Send the Read command (0x20)
    io_outb(ATA_STATUS, 0x20); io_wait();  // Read Sectors command
    
    // Wait until the drive is ready
    ata_wait_ready();

    // Read the data from the data register (16-bit word at a time)
    for (uint32_t i = 0; i < sectors * 256; i++) {
        uint16_t data = io_inw(ATA_DATA); io_wait();  // Read data (16-bit)
        buffer[i * 2] = (uint8_t)(data & 0xFF);
        buffer[i * 2 + 1] = (uint8_t)((data >> 8) & 0xFF);  // Store the lower byte in the buffer
        
    }
}
