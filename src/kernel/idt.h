#ifndef IDT_H
#define IDT_H
#include <stdint.h>

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs; /* kernel seg here */
    uint8_t  always0;   /* always 0 */  
    uint8_t  attributes;/* set with above table */
    uint16_t isr_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void idt_load();
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_install();

#endif
