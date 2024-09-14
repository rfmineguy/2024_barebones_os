#ifndef GDT_H
#define GDT_H
#include <stdint.h>
// https://www.youtube.com/watch?v=jwulDRMQ53I
// https://wiki.osdev.org/Global_Descriptor_Table

struct gdt_entry {
    uint16_t limit;
    uint16_t baselo;
    uint8_t  basemid;
    uint8_t  access;
    uint8_t  flags_limit;
    uint8_t  basehi;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    unsigned int base;
} __attribute__((packed));

void gdt_init();
void gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

#endif
