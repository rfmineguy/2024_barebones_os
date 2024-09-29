#include "serial.h"
#include "io.h"
#include "log.h"
#include "printf.h"

#define PORT 0x3f8 // com1

int serial_init() {
   io_outb(PORT + 1, 0x00);    // Disable all interrupts
   io_outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   io_outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   io_outb(PORT + 1, 0x00);    //                  (hi byte)
   io_outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   io_outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   io_outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   io_outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   io_outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

   // Check if serial is faulty (i.e: not same byte as sent)
   if(io_inb(PORT + 0) != 0xAE) {
      return 1;
   }

   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   io_outb(PORT + 4, 0x0F);
   log_info("SerialInit ", "Sucessfully initialized\n");
   return 0;
}

char serial_read(){
    while (serial_received() == 0);
    return io_inb(PORT);
}
int serial_is_transmit_empty(){
    return io_inb(PORT + 5) & 0x20;
}

void serial_write_ch(char ch){
    while (serial_is_transmit_empty() == 0);
    io_outb(PORT, ch);
}
void serial_write_str(char* ch){
    while (*ch) {
        serial_write_ch(*ch);
        ch++;
    }
}
void serial_printf(const char* fmt, ...) {
    static char buf[1000] = {0};
    va_list alist;
    va_start(alist, fmt);
    int x = k_vsprintf(buf, fmt, alist);
    va_end(alist);
    buf[x] = 0;
    serial_write_str(buf);
}
int  serial_received(){
    return io_inb(PORT);
}
