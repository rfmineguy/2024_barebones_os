#include "ata.h"
#include "io.h"
#include "log.h"

// credit to https://wiki.osdev.org/ATA_read/write_sectors
void ata_wait_ready(){
    while (io_inb(ATA_STATUS) & 0x80) {}
}
void ata_select_drive(uint8_t drive) {
    io_outb(ATA_DRIVE_HEAD, drive);
    io_wait();
}
void ata_soft_reset() {
    io_outb(0x3F6, 0x04); // Send software reset
    io_wait();
    io_outb(0x3F6, 0x00); // Clear reset
    ata_wait_ready();
}
void ata_read(int drive, int lba, uint8_t* buffer, uint32_t sectors){
    log_group_begin("ATA Read");
    log_info("Drive  ", "%x", drive);
    log_info("LBA    ", "%d", lba);
    log_info("AbsAddr", "dec (%d), hex(0x%x)", lba * 512, lba * 512);
    log_info("Sectors", "%d", sectors);
    log_info("buf_add", "%x", buffer);
    log_group_end("ATA Read");

    ata_soft_reset();

    // Wait for the drive to be ready
    ata_wait_ready();

    // Select the drive (e.g., Primary Master)
    ata_select_drive(drive);  // Primary Master (or choose another drive)

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

    // Read the data from the data register (16-bit word at a time, 256*2 = 512)
    for (uint32_t i = 0; i < sectors * 256; i++) {
        uint16_t data = io_inw(ATA_DATA); io_wait();  // Read data (16-bit)
        buffer[i * 2] = (uint8_t)(data & 0xFF);
        buffer[i * 2 + 1] = (uint8_t)((data >> 8) & 0xFF);  // Store the lower byte in the buffer
    }
}

void ata_write(int drive, int lba, uint8_t* buffer, uint32_t sectors) {
    ata_wait_ready();
    ata_select_drive(drive);
    io_outb(0x1F2, sectors); io_wait();
    // Set the LBA address
    io_outb(0x1F3, lba & 0xFF);         io_wait();  // LBA Low
    io_outb(0x1F4, (lba >> 8) & 0xFF);  io_wait();  // LBA Mid
    io_outb(0x1F5, (lba >> 16) & 0xFF); io_wait();  // LBA High
    // Send the Write command (0x20)
    io_outb(ATA_STATUS, 0x30); io_wait();  // Write Sectors command
    ata_wait_ready();

    io_inb(0x1F7);

    for (uint32_t i = 0; i < sectors * 256; i++) {
        uint16_t data = (uint16_t)(buffer[i * 2] | (buffer[i * 2 + 1] << 8));  
        io_outw(0x1F0, data);

        uint8_t err = io_inb(ATA_ERROR); io_wait();
        if (err) {
            if (err & (1 << 1)) log_crit("ATA Read", "Sector not found: %d\n", lba);
            if (err & (1 << 3)) log_crit("ATA Read", "Uncorrectable error\n");
            if (err & (1 << 0)) log_crit("ATA Read", "Bad block detected\n");
        }
    }
}

/*
 * Error Checking in ata_read and ata_write
 * Both ata_read and ata_write should ideally return a status code indicating success or failure. If the functions are failing silently, adding error checking can help track down what’s going wrong.
 * 
 * ata_read/ata_write return value: You can modify these functions to return 0 for success and non-zero values for errors, such as invalid sector, drive not ready, or hardware errors.
 * 
 * Status Register Check: After each read or write operation, check the ATA
 * status register (0x1F7) for any error flags. If there's an error, you can
 * read the error register (0x1F1) for more information.
*/
