#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"

void kernel_main(void) {
    serial_init();
    serial_write_ch('a');
    vga_init();

    idt_cli();
    gdt_init();
    vga_writestring("Installed gdt\n");

    idt_install();

    vga_writestring("Hello\n");
    vga_writestring("Hello 2\n");
    vga_putch('d');
}
