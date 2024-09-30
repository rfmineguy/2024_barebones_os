#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "timer.h"
#include "keyboard.h"
#include "../stdlib/printf.h"

#define R(x) #x

void kernel_main(void) {
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
    gdt_init();
    k_printf("Installed GDT\n");
    idt_install();
    k_printf("Installed IDT\n");
    timer_init();
    k_printf("Initialized timer\n");
    keyboard_init();
    k_printf("Initialized keyboard\n");

    k_printf("Everything initialized!\n");

    for(;;);
}
