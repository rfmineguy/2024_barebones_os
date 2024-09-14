.global io_outb
.global io_inb
.global io_wait

// void outb(uint16_t, uint8_t)
io_outb:
    xor %edx, %edx
    movl 4(%esp), %edx
    movb 8(%esp), %al 
    out %al, %dx
    ret

// void inb()
io_inb:
    xor %edx, %edx
    movl 4(%esp), %edx
    in %dx, %al
    ret

// void iowait()
io_wait:
    jmp wait
wait:
    ret
    
