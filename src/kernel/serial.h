#ifndef SERIAL_H
#define SERIAL_H

int  serial_init();

char serial_read();
int  serial_is_transmit_empty();

void serial_write_ch(char);
int  serial_write_str(char*);
int  serial_printf(const char* fmt, ...);
void serial_log(int, const char* fmt, ...);

int  serial_received();

#endif
