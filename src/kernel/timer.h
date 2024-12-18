#ifndef TIMER_H
#define TIMER_H
#include "int_regs.h"

//https://wiki.osdev.org/Programmable_Interval_Timer
void timer_add_listener(int(*)(int), int);

void timer_init();
void timer_onirq0(struct interrupt_registers_test*);

uint64_t timer_ticks();

#endif
