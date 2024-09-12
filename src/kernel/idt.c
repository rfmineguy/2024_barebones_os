#include "idt.h"
#include <stdbool.h>

// https://wiki.osdev.org/Interrupts_Tutorial
struct idt_entry idt[256] = {0};
struct idt_ptr   idtp;

static bool vectors[256];
extern void* isr_stub_table[];

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    struct idt_entry* e = &idt[vector];

    e->isr_low = (uint32_t)isr & 0xFFFF; // warning on 64 bit host is normal
    e->kernel_cs = 0x08; // this is the kernel code selector offset in gdt
    e->attributes = flags;
    e->isr_high = (uint32_t)isr >> 16;
    e->always0 = 0;
}

void idt_install() {
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) &idt[0];

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        vectors[vector] = true;
    }

    idt_load();
}

__attribute__((noreturn))
void exception_handler(void);
void exception_handler() {
    __asm__ volatile ("cli; hlt");
}
