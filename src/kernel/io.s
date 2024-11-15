.global io_outb
.global io_inb
.global io_inw
.global io_wait

io_outb:
    xor %edx, %edx
    movl 4(%esp), %edx
    movb 8(%esp), %al
    out %al, %dx
    ret

io_inb:
    xor %edx, %edx
    movl 4(%esp), %edx
    in %dx, %al
    ret

io_inw:
    xor %edx, %edx
    movl 4(%esp), %edx
    in %dx, %ax
    ret

io_wait:
    jmp io_wait_jmp
io_wait_jmp:
    ret
    
