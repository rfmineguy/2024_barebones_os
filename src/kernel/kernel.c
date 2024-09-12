#include "vga.h"
#include "gdt.h"

void kernel_main(void) {
    vga_init();
    gdt_install();
    vga_writestring("Installed gdt\n");

    vga_writestring("Hello\n");
    vga_writestring("Hello 2\n");
    vga_putch('d');
}
