/* CONSTANTS FOR MULTIBOOT HEADER */
.set ALIGN,     1<<0             /* align modules on page boundaries */
.set MEMINFO,   1<<1             /* provide memory map */
.set FLAGS,     ALIGN | MEMINFO  /* multiboot flag field */
.set MAGIC,     0x1BADB002       /* lets multiboot find the header */
.set CHECKSUM,  -(MAGIC + FLAGS) /* proves we are multiboot */

/* MULTIBOOT HEADER */
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0
.long 0
.long 0
.long 0
.long 0
.long 0
.long 1024
.long 768
.long 32


/* STACK AREA */
.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

/* ENTRY POINT */
.section .text
.global _start
.type _start, @function
_start:
    /* paging disabled, interrupts disabled (by bootloader) */
    /* setup stack */
    mov $stack_top, %esp
    
    /* initialize processor state before kernel is entered */
    /* floating point instructions not enabled yet */
    /* should setup gdt here */
    /* should setup paging here */

    /* enter high-level kernel */
    /* ebx and eax contain multiboot header flags */
    pushl %ebx
    pushl %eax
    call kernel_main

    /* loop infinitely */
    cli /* disable interrupts */
1:  hlt
    jmp 1b

.size _start, . - _start
