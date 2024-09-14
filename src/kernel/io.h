// https://wiki.osdev.org/Inline_Assembly/Examples#I/O_access

#ifndef IO_H
#define IO_H
#include <stdint.h>

void    io_outb(uint16_t port, uint8_t val);
uint8_t io_inb(uint16_t port);

void    io_wait();

#endif
