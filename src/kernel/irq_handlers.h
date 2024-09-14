#ifndef IRQ_HANDLER_H
#define IRQ_HANDLER_H

// assembly definitions
extern void irq_asm_default();
extern void irq_asm_keyboard();
extern void irq_asm_timer();

void irq_default();
void irq_keyboard();
void irq_timer();

#endif
