.global idt_load
.extern idtp
idt_load:
    lidt [idtp]  // load new idt
    sti          // turn on interrupts
    ret
