#ifndef PIC_H
#define PIC_H
#include <stdint.h>

#define PIC1         0x20
#define PIC2         0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA    (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA    (PIC2 + 2)

#define PIC_EOI      0x20 // command code for End-Of-Interrupt

void pic_send_eoi(uint8_t irq);
void pic_remap();
void pic_setflags(uint8_t pic1, uint8_t pic2);

#endif
