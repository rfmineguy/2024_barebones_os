#include "vga.h"
#include "gdt.h"
#include "idt.h"

void kernel_main(void) {
    vga_init();
    gdt_install();
    vga_writestring("Installed gdt\n");
    idt_install();
    vga_writestring("Installed idt\n");

    vga_writestring("Hello\n");
    vga_writestring("Hello 2\n");
    vga_putch('d');
}
