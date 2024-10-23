#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "timer.h"
#include "keyboard.h"
#include "multiboot.h"
#include "memory.h"
#include "../stdlib/printf.h"

#define R(x) #x
#define UNUSED(x) (void)(x)

static int i = 0;
void kernel_main(uint32_t magic, struct multiboot_info* bootinfo) {
    UNUSED(magic);

    serial_init();
    vga_init();

    k_printf(R("+=====================================+") "\n");
    k_printf(R("|          ___  ________  ____        |") "\n");
    k_printf(R("|         / _ \/ __/ __ \/ __/        |") "\n");
    k_printf(R("|        / , _/ _// /_/ /\ \          |") "\n");
    k_printf(R("|       /_/|_/_/  \____/___/          |") "\n");
    k_printf(R("|                                     |") "\n");
    k_printf(R("+=====================================+") "\n");

    int x = 972656;
    k_printf("%d, %d, %d, %d\n", x, x, x, 972656);

    idt_cli();

    // Initialize hardware (sets up exception handlers as well)
    gdt_init();            k_printf("Installed GDT\n");
    idt_install();         k_printf("Installed IDT\n");

    // Setup irq handlers
    timer_init();          k_printf("Initialized timer\n");
    keyboard_init();       k_printf("Initialized keyboard\n");
    // memory_init(bootinfo); k_printf("Initialized memory\n");

    idt_sti();
    k_printf("Everything initialized!\n");

    int bank = 0;
    int cursorx = 0, cursory = 0;
    for(;;) {
        if (timer_ticks() % 20 == 0) {
            if (i % 3 == 0) vga_put_ch_at('-', 20, 20);
            if (i % 3 == 1) vga_put_ch_at('/', 20, 20);
            if (i % 3 == 2) vga_put_ch_at('|', 20, 20);
        }
        i++;
    }
}
