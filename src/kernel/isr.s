.macro isr_err name
.global isr\name
isr\name:
    cli
    mov $\name, %eax
    pushl %eax
    // pushl \name
    call isr_common_stub
.endm

.macro isr_no_err name
.global isr\name
isr\name:
    cli
    mov $0, %eax
    pushl %eax
    mov $\name, %eax
    pushl %eax
    // pushl 0
    // pushl \name
    call isr_common_stub
.endm

.extern exception_handler
isr_no_err 0
isr_no_err 1
isr_no_err 2
isr_no_err 3
isr_no_err 4
isr_no_err 5
isr_no_err 6
isr_no_err 7

isr_err    8
isr_no_err 9
isr_err    10
isr_err    11
isr_err    12
isr_err    13
isr_err    14

isr_no_err 15
isr_no_err 16
isr_no_err 17
isr_no_err 18
isr_no_err 19
isr_no_err 20
isr_no_err 21
isr_no_err 22
isr_no_err 23
isr_no_err 24
isr_no_err 25
isr_no_err 26
isr_no_err 27
isr_no_err 28
isr_no_err 29
isr_no_err 30
isr_no_err 31
isr_no_err 128
isr_no_err 177

isr_common_stub:
    pusha
    mov %ds, %eax
    pushl %eax
    mov %cr2, %eax
    pushl %eax
    mov $10, %ax
    
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    
    pushl %esp
    call isr_handler
    
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
