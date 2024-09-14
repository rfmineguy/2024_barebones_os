#include "gdt.h"

extern void gdt_flush();

struct gdt_entry gdt_entries[5];
struct gdt_ptr gdt_ptr;

void gdt_init(){
    gdt_ptr.limit = (sizeof (struct gdt_entry) * 5) - 1;
    gdt_ptr.base = (unsigned int)&gdt_entries[0];

    // see https://wiki.osdev.org/Global_Descriptor_Table for the access explanation
    gdt_set_gate(0, 0, 0, 0, 0);                // null segment
    gdt_set_gate(1, 0, 0xffffffff, 0x9A, 0xCF); // kernel code segment
    gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xCF); // kernel data segment
    gdt_set_gate(3, 0, 0xffffffff, 0xFA, 0xCF); // user   code segment
    gdt_set_gate(4, 0, 0xffffffff, 0xF2, 0xCF); // user   data segment

    gdt_flush();
}

void gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    struct gdt_entry* e = &gdt_entries[n];
    e->baselo  = base & 0xffff;
    e->basemid = (base >> 16) & 0xff;
    e->basehi  = (base >> 24) & 0xff;

    e->limit = limit & 0xffff;
    e->flags_limit = (limit >> 16) & 0x0f;
    e->flags_limit |= (gran & 0xf0);

    e->access = access;
}
