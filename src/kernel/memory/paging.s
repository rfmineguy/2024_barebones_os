.global paging_enable
.global paging_load_pagedir
.global paging_reload

paging_enable:
	mov %cr0, %eax
	or $0x80000011, %eax
	mov %eax, %cr0
	ret

paging_load_pagedir:
	push %ebp
	mov %esp, %ebp
	mov 8(%esp), %eax
	mov %eax, %cr3
	mov %ebp, %esp
	pop %ebp
	ret

paging_reload:
  mov	%cr3,%eax
	mov	%eax,%cr3
  ret
