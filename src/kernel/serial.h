#ifndef SERIAL_H
#define SERIAL_H
#define bool int
#define true 1
#define false 0

int  serial_init();

char serial_read();
int  serial_is_transmit_empty();

void serial_write_ch(char);
void serial_write_str(char*);
int  serial_received();

#endif
