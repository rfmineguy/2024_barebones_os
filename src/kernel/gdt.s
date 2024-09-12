.global gdt_flush
.extern gp

gdt_flush:
    lgdt [gp]      /* load the gdt in 'gp' to the gdt register */
    mov $0x10, %ax   /* offset to data segment */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    jmp $0x08,$flush2 /* far jump to the code segment */

flush2:
    ret
