#include "idt.h"
#include "memset.h"

struct idt_entry idt[256] = {0};
struct idt_ptr   idtp;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags){
    idt[num].always0 = 0;
    idt[num].base_hi = (uint16_t) (base >> 32);
    idt[num].base_low = base & 0xffff0000;
    idt[num].flags = flags;
    idt[num].sel = sel;
}

void idt_install() {
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) &idt;
}

