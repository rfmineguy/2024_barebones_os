.global gdt_flush
.global tss_flush
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

tss_flush:
    mov $0x2B, %ax //tss offset
    ltr %ax        //load task state register
    ret
