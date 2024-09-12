#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct idt_entry {
    uint16_t base_low;
    uint16_t sel;       /* kernel seg here */
    uint8_t  always0;   /* always 0 */  
    uint8_t  flags;     /* set with above table */
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void idt_load();
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
#endif
