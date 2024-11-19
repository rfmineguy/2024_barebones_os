#ifndef MOUSE_H
#define MOUSE_H
#include "int_regs.h"

void mouse_add_listener(int(*)(char)); // not implemented

void mouse_init();
void mouse_irq(struct interrupt_registers_test*);

#endif
