/* CONSTANTS FOR MULTIBOOT HEADER */
.set ALIGN,     1<<0                     /* align modules on page boundaries */
.set MEMINFO,   1<<1                     /* provide memory map */
.set DRIVES,    1<<7                     /* provide drive info */
.set FLAGS,     DRIVES /*ALIGN | MEMINFO | DRIVES /* multiboot flag field */
.set MAGIC,     0x1BADB002               /* lets multiboot find the header */
.set CHECKSUM,  -(MAGIC + FLAGS)         /* proves we are multiboot */

/* MULTIBOOT 2 HEADER */
.section .multiboot
.align 8
mboot_header:
.long 0xE85250D6    /* magic */
.long 0             /* architecture */
.long (mboot_header_end - mboot_header)    /* length of header */
.long -(0xE85250D6 + 0 + (mboot_header_end - mboot_header))
// .long 0x10          /* tags are 16 bytes from the start */

mboot_tags:
//.align 8          /* 8 byte alignment */
//.long  0x05       /* 1=information request */
//.long  0x20
//.long  0
//.long  0
//.long  0

# Tags are terminated by a tag of type ‘0’ and size ‘8’.
.align  8
.short  0
.short  0
.short  8
mboot_header_end:

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
/*https://www.gnu.org/software/grub/manual/multiboot2/html_node/Machine-state.html*/
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
    lea mboot_header, %ecx
    pushl %ecx /* header */
    /* pushl %ebx /* info */
    pushl %eax /* magic */ /* should be 0x36d76289 */
    call kernel_main

    /* loop infinitely */
    cli /* disable interrupts */
1:  hlt
    jmp 1b

.size _start, . - _start

