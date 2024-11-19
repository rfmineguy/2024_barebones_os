#include "mouse.h"
#include "idt.h"
#include "log.h"

void mouse_add_listener(int(*listener)(char)) {
}

void mouse_init() {
    idt_cli();
    irq_install_handler(12, &mouse_irq);
}
void mouse_irq(struct interrupt_registers_test* regs) {
    log_info("Mouse", "Test");
}
