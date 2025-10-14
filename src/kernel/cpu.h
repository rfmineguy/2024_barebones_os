#ifndef CPU_H
#define CPU_H
#include <stdint.h>

// https://www.eecg.utoronto.ca/~amza/www.mindsec.com/files/x86regs.html

void get_registers();
extern void cpu_halt();
extern uint32_t cpu_get_eax();
extern uint32_t cpu_get_ebx();
extern uint32_t cpu_get_ecx();
extern uint32_t cpu_get_edx();
extern uint32_t cpu_get_cs();
extern uint32_t cpu_get_ds();
extern uint32_t cpu_get_es();
extern uint32_t cpu_get_fs();
extern uint32_t cpu_get_gs();
extern uint32_t cpu_get_ss();
extern uint32_t cpu_get_esi();
extern uint32_t cpu_get_edi();
extern uint32_t cpu_get_ebp();
extern uint32_t cpu_get_esp();
extern uint32_t cpu_get_cr2();
extern uint32_t cpu_get_eip();

#endif
