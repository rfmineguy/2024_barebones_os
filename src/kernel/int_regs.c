#include "int_regs.h"
#include "serial.h"
#include "log.h"

void int_regs_log_decimal(struct interrupt_registers_test* regs) {
	log_info("", "cr2   =%10d", regs->cr2);
	log_info("", "eax   =%10d ebx     =%10d ecx   =%10d edx    =%10d", 
					regs->eax, regs->ebx, regs->ecx, regs->edx);
	log_info("", "edi   =%10d esi     =%10d ebp   =%10d esp    =%10d",
					regs->edi, regs->edi, regs->esi, regs->ebp, regs->esp);
	log_info("", "int_no=%10d err_code=%10d",
					regs->int_no, regs->err_code);
	log_info("", "eip   =%10d cs      =%10d eflags=%10d useresp=%10d ss=%10d",
					regs->eip, regs->cs, regs->eflags, regs->useresp, regs->ss);
}

void int_regs_log_hex(struct interrupt_registers_test* regs) {
	log_info("", "cr2   =%10x", regs->cr2);
	log_info("", "eax   =%10x ebx     =%10x ecx   =%10x edx    =%10x", 
					regs->eax, regs->ebx, regs->ecx, regs->edx);
	log_info("", "edi   =%10x esi     =%10x ebp   =%10x esp    =%10x",
					regs->edi, regs->edi, regs->esi, regs->ebp, regs->esp);
	log_info("", "int_no=%10x err_code=%10x",
					regs->int_no, regs->err_code);
	log_info("", "eip   =%10x cs      =%10x eflags=%10x useresp=%10x ss=%10x",
					regs->eip, regs->cs, regs->eflags, regs->useresp, regs->ss);
}

void int_regs_log(struct interrupt_registers_test* regs, enum print_type type) {
	log_group_begin("Registers");
	switch (type) {
		case DECIMAL: int_regs_log_decimal(regs); break;
		case HEX: int_regs_log_hex(regs); break;
	}
	log_group_end("Registers");
}
