#include "irq_handlers.h"
#include "vga.h"

void irq_default(){}

void irq_keyboard(){
    vga_putch('k');
}
void irq_timer(){
    vga_putch('_');
}
