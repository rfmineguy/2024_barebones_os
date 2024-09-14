.global idt_load
.global idt_cli
.global idt_sti
.extern idtp
idt_load:
    lidt [idtp]  // load new idt
    ret

idt_cli:
    cli
    ret

idt_sti:
    sti
    ret
