#include "idt.h"
#include "pic.h"
#include "vga.h"
#include "log.h"
#include "io.h"
#include "../stdlib/printf.h"
#include "../stdlib/memset.h"
#include <stdbool.h>

// https://wiki.osdev.org/Interrupts_Tutorial
// https://wiki.osdev.org/Interrupts
//      - maps irqs to vector numbers under "General IBM-PC Compatible Interrupt Information"
// https://wiki.osdev.org/Exceptions

struct idt_entry idt_entries[256];
struct idt_ptr   idtp;

extern void idt_flush();

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    struct idt_entry* e = &idt_entries[num];
    e->base_low = base & 0xffff;
    e->base_high = (base >> 16) & 0xffff;
    e->always0 = 0;
    e->sel = sel;
    e->flags = flags | 0x60;
    log_info("IDT SetGate", "#%d, base: %x, sel: %d, flags: %b", num, base, sel, flags);
}

void idt_install() {
    log_group_begin("IDT Install");
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) &idt_entries;
    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);

    pic_remap();

    // 0x8E = 1000 1110
    // 0x08 = 0000 1000 ; code segment
    idt_set_gate(0,  (uint32_t) isr0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t) isr1,  0x08, 0x8E);
    idt_set_gate(2,  (uint32_t) isr2,  0x08, 0x8E);
    idt_set_gate(3,  (uint32_t) isr3,  0x08, 0x8E);
    idt_set_gate(4,  (uint32_t) isr4,  0x08, 0x8E);
    idt_set_gate(5,  (uint32_t) isr5,  0x08, 0x8E);
    idt_set_gate(6,  (uint32_t) isr6,  0x08, 0x8E);
    idt_set_gate(7,  (uint32_t) isr7,  0x08, 0x8E);
    idt_set_gate(8,  (uint32_t) isr8,  0x08, 0x8E);
    idt_set_gate(9,  (uint32_t) isr9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t) isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t) isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t) isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t) isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t) isr14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t) isr15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t) isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t) isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t) isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t) isr19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t) isr20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t) isr21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t) isr22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t) isr23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t) isr24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t) isr25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t) isr26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t) isr27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t) isr28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t) isr29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t) isr30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t) isr31, 0x08, 0x8E);
    
    idt_set_gate(32, (uint32_t) irq0,  0x08, 0x8E);
    idt_set_gate(33, (uint32_t) irq1,  0x08, 0x8E);
    idt_set_gate(34, (uint32_t) irq2,  0x08, 0x8E);
    idt_set_gate(35, (uint32_t) irq3,  0x08, 0x8E);
    idt_set_gate(36, (uint32_t) irq4,  0x08, 0x8E);
    idt_set_gate(37, (uint32_t) irq5,  0x08, 0x8E);
    idt_set_gate(38, (uint32_t) irq6,  0x08, 0x8E);
    idt_set_gate(39, (uint32_t) irq7,  0x08, 0x8E);
    idt_set_gate(40, (uint32_t) irq8,  0x08, 0x8E);
    idt_set_gate(41, (uint32_t) irq9,  0x08, 0x8E);
    idt_set_gate(42, (uint32_t) irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint32_t) irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint32_t) irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint32_t) irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint32_t) irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint32_t) irq15, 0x08, 0x8E);

    idt_set_gate(128, (uint32_t) isr128, 0x08, 0x8E);
    idt_set_gate(177, (uint32_t) isr177, 0x08, 0x8E);

    idt_flush();

		for (int i = 0; i < 32; i++) {
			isr_install_handler(i, unimplemented_handler);
		}
    isr_install_handler(0x00, div_zero_handler);
    isr_install_handler(0x0E, page_fault_handler);
    log_group_end("IDT Install");
}

void div_zero_handler(struct interrupt_registers_test* regs) {
    log_info("Handler", "Division by zero error", regs->int_no);
}

void page_fault_handler(struct interrupt_registers_test* regs) {
    log_info("Handler", "WIP");
}

void unimplemented_handler(struct interrupt_registers_test* regs) {
    log_info("Handler", "Unimplemented");
}

// https://wiki.osdev.org/Exceptions
const char* exception_messages[] = {
    [0] = "Division by zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into dectected overflow",
    "Out of bounds",
    "Invalid opcode",
    "No coprocessor",
    "Double fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Stack fault",
    "General protection fault",
    "Page fault",
    "Unknown interrupt",
    "Coprocessor fault",
    "Alignment fault",
    "Machine check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

void *exception_handlers[32] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// https://wiki.osdev.org/Exceptions
void isr_handler(struct interrupt_registers_test* regs) {
    log_warn("ISR", "#%d", regs->int_no);
    if (regs->int_no < 32) {
        void (*handler)(struct interrupt_registers_test* regs) = exception_handlers[regs->int_no];
        if (handler) handler(regs);
        else {
            log_crit("ISR", "Unhandled isr %d", regs->int_no);
        }
    }
    // for(;;);
}

extern void mouse_wait(uint8_t a_type);
void idt_debug_setup() {
    // Check PIC masks
    uint8_t pic1_mask = io_inb(0x21);
    uint8_t pic2_mask = io_inb(0xA1);
    log_info("Debug", "PIC masks - Master: %02x, Slave: %02x", pic1_mask, pic2_mask);

    // Verify mouse isn't masked
    if (pic2_mask & (1 << 4)) { // IRQ12 is bit 4 on slave PIC
        log_info("Debug", "Warning: Mouse IRQ is masked!");
    }

    // Read 8042 controller config byte
    mouse_wait(1);
    io_outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t ps2_config = io_inb(0x60);
    log_info("Debug", "PS/2 Controller config: %02x", ps2_config);
    
    if (!(ps2_config & (1 << 1))) {
        log_info("Debug", "Warning: Mouse IRQ disabled in PS/2 controller!");
    }
}

void isr_install_handler(int isr, void(*handler_func)(struct interrupt_registers_test*)) {
    exception_handlers[isr] = handler_func;
    log_info("ISR Install", "Installed isr handler #%d, %x", isr, handler_func);
}
void isr_uninstall_handler(int isr) {
    exception_handlers[isr] = 0;
}

void *irq_routines[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler_func)(struct interrupt_registers_test*)) {
    irq_routines[irq] = handler_func;
    log_info("IRQ Install", "Installed irq handler #%d, %x", irq, handler_func);
}

void irq_uninstall_handler(int irq) {
    irq_routines[irq] = 0;
}

void irq_handler(struct interrupt_registers_test* regs) {
    void (*handler)(struct interrupt_registers_test* regs);
		if (regs->int_no - 32 <= 0) return;
    handler = irq_routines[regs->int_no - 32];
		switch (regs->int_no - 32) {
			case 0: break;
			default: break;///log_info("IRQ", "#%d", regs->int_no);
		}
    if (handler) handler(regs);
    if (regs->int_no >= 40) {
        io_outb(0xA0, 0x20); io_wait();
    }
    io_outb(0x20, 0x20); io_wait();
}
