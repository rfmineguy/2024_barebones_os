#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "printf.h"

void kernel_main(void) {
    serial_init();
    vga_init();

    idt_cli();
    gdt_init();
    printf("Installed gdt\n");

    uint8_t b = 0x0;
    printf("PIC: %d\n", b);
    idt_install();
    printf("Installed idt\n");


    vga_putch(1/0);


    // vga_writestring("Hello\n");
    // vga_writestring("Hello 2\n");
    // vga_putch('d');
}
