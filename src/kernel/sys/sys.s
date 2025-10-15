.global sys_halt

sys_halt:
    hlt
    jmp sys_halt
