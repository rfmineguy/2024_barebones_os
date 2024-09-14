#include "idt.h"
#include "pic.h"
#include "irq_handlers.h"
#include <stdbool.h>

// https://wiki.osdev.org/Interrupts_Tutorial
struct idt_entry idt[256] = {0};
struct idt_ptr   idtp;

static bool vectors[256];
extern void* isr_stub_table[];

void idt_set_gate(uint8_t vector, uint32_t handler, uint16_t selector, uint8_t flags) {
    struct idt_entry* e = &idt[vector];
    e->isr_low = handler & 0xffff;
    e->isr_high = (handler >> 16) & 0xffff;
    e->kernel_cs = selector;
    e->always0 = 0;
    e->attributes = flags;
}

void idt_install() {
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) &idt[0];

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_gate(vector, (uint32_t) &irq_default, 0x08, 0x8e);
        vectors[vector] = true;
    }

    idt_set_gate(0x20, (uint32_t)&irq_asm_timer, 0x08, 0x8e);
    idt_set_gate(0x21, (uint32_t)&irq_asm_keyboard, 0x08, 0x8e);

    pic_remap(0x20, 0x28); // pic interrupts mapped from 0x20 - 0x30
    pic_setflags((uint8_t)~0b10000000, ~0b00000000);
    idt_load(); // load idtp
    idt_sti();  // enable interrupts
}

__attribute__((noreturn))
void exception_handler(void);
void exception_handler() {
    __asm__ volatile ("cli; hlt");
}
