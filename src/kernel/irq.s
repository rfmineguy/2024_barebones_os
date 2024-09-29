.macro irq name arg
    .global irq\name
    irq\name:
        cli
        push $0x0
        push $\arg
        jmp irq_common_stub_test
.endm
    
irq 0,  0x20
irq 1,  0x21
irq 2,  0x22
irq 3,  0x23
irq 4,  0x24
irq 5,  0x25
irq 6,  0x26
irq 7,  0x27
irq 8,  0x28
irq 9,  0x29
irq 10, 0x2a
irq 11, 0x2b
irq 12, 0x2c
irq 13, 0x2d
irq 14, 0x2e
irq 15, 0x2f

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

irq_common_stub_test:
    pusha
    
    // mov $0x10, %ax
    // mov %ax, %ds
    // mov %ax, %es
    // mov %ax, %fs
    // mov %ax, %gs
    // 
    pushl %esp
    call irq_handler
    
    // add $8, %esp
    // pop %ebx
    // mov %bx, %ds
    // mov %bx, %es
    // mov %bx, %fs
    // mov %bx, %gs
    
    popa
    add $8, %esp
    sti
    iret
