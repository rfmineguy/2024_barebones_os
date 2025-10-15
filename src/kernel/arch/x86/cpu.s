.macro cpu_get reg
.global cpu_get_\reg
cpu_get_\reg:
	mov %\reg, %eax
	ret
.endm

cpu_get eax
cpu_get ebx
cpu_get ecx
cpu_get edx
cpu_get cs
cpu_get ds
cpu_get es
cpu_get fs
cpu_get gs
cpu_get ss
cpu_get esi
cpu_get edi
cpu_get ebp
cpu_get esp
cpu_get cr2

.global cpu_get_eip
cpu_get_eip:
	cli
	call get_eip_
get_eip_:
	pop %eax
	sti
	ret

.global cpu_halt
cpu_halt:
	cli
cpu_halt_1:
	hlt
	jmp cpu_halt_1
