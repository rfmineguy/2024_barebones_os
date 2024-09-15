#include "pic.h"
#include "io.h"
#include "vga.h"

#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) io_outb(PIC2_COMMAND, PIC_EOI);
    io_outb(PIC1_COMMAND, PIC_EOI);
}

void pic_remap() {
    // uint8_t a1, a2;

    // Save pic masks
    // a1 = io_inb(PIC1_DATA);
    // a2 = io_inb(PIC2_DATA);

    // Start initialize sequence
    io_outb(PIC1_COMMAND, 0x11); io_wait();
    io_outb(PIC2_COMMAND, 0x11); io_wait();

    // PIC vector offset
    //   1 - master
    //   2 - slave
    io_outb(PIC1_DATA, 0x20); io_wait();
    io_outb(PIC2_DATA, 0x28); io_wait();

    // tell master pic there is a slave pic at IRQ2 (0000 0100)
    io_outb(PIC1_DATA, 0x04); io_wait();

    // tell slave pic its cascade identity (0000 0010)
    io_outb(PIC2_DATA, 0x02); io_wait();

    // 8086 mode
    io_outb(PIC1_DATA, 0x01); io_wait();
    io_outb(PIC2_DATA, 0x01); io_wait();

    // restore flags
    io_outb(PIC1_DATA, 0x00); io_wait();
    io_outb(PIC2_DATA, 0x00); io_wait();
}

void pic_setflags(uint8_t pic1, uint8_t pic2) {
    io_outb(PIC1_DATA, pic1); io_wait();
    io_outb(PIC2_DATA, pic2); io_wait();
    vga_writestring("Set pic flags\n");
}
