#include "sys.h"
#include "io.h"

void sys_reboot() {
    uint8_t good = 0x02;
    while (good & 0x02)
        good = io_inb(0x64);
    io_outb(0x64, 0xFE);
    sys_halt();
}

// sys_halt is defined in src/kernel/sys.s
