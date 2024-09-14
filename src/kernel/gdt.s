.global gdt_flush
.extern gdt_ptr

gdt_flush:
    cli
    lgdt [gdt_ptr]
    mov $0x10, %eax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x08,$flush
flush:
    ret
