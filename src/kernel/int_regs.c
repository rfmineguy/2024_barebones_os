#include "int_regs.h"
#include "serial.h"

void int_regs_show_serial(struct interrupt_registers_test* regs) {
    serial_printf("eax=%d ebx=%d ecx=%d edx=%d", 
            regs->eax, regs->ebx, regs->ecx, regs->edx);
    serial_printf("edi=%d esi=%d ebp=%d esp=%d ebx=%d edx=%d ecx=%d eax=%d\n",
            regs->edi, regs->edi, regs->esi, regs->ebp, regs->esp, regs->ebx, regs->edx, regs->ecx, regs->eax);
    serial_printf("int_no=%d err_code=%d\n",
            regs->int_no, regs->err_code);
    serial_printf("eip=%d cs=%d eflags=%d useresp=%d ss=%d",
            regs->eip, regs->cs, regs->eflags, regs->useresp, regs->ss);
}
