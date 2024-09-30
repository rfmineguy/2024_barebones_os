#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "int_regs.h"

void keyboard_init();
void keyboard_irq(struct interrupt_registers_test*);

#endif
