.macro irq name arg
    .global irq\name
    irq\name:
        cli
        pushl 0
        pushl \arg
        jmp irq_common_stub
.endm
    
irq 0,  32
irq 1,  33
irq 2,  34
irq 3,  35
irq 4,  36
irq 5,  37
irq 6,  38
irq 7,  39
irq 8,  40
irq 9,  41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

.extern irq_handler
irq_common_stub:
    pusha
    mov %ds, %eax
    pushl %eax
    mov %cr2, %eax
    pushl %eax
    
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    pushl %esp
    call irq_handler
    
    add $8, %esp
    pop %ebx
    mov %bx, %ds
    mov %bx, %es
    mov %bx, %fs
    mov %bx, %gs
    
    popa
    add $8, %esp
    sti
    iret
