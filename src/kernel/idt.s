.global idt_flush
.global idt_cli
.global idt_sti
.extern idtp
idt_flush:
    lidt [idtp]  // load new idt
    sti
    ret

idt_cli:
    cli
    ret

idt_sti:
    sti
    ret

// isr definitions in isr.s
