#ifndef INT_REGS_H
#define INT_REGS_H
#include <stdint.h>

struct interrupt_registers {
    uint32_t cr2;
    uint32_t ds;                                     // 
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
    uint32_t int_no, err_code;                       // pushed by isr_handler
    uint32_t eip, cs, eflags, useresp, ss;           // pushed by cpu
};
struct interrupt_registers_test {
		uint32_t cr2;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
		// uint32_t gs, fs, es, ds;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

enum print_type {
	HEX, DECIMAL,
};

void int_regs_log(struct interrupt_registers_test*, enum print_type);

#endif
