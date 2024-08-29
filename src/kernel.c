#include "vga.h"

void kernel_main(void) {
    vga_init();
    vga_writestring("Hello\n");
}
