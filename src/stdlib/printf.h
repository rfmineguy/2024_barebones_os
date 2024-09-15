#ifndef PRINTF_H
#define PRINTF_H
#include "../kernel/vga.h"
#include <stdarg.h>

typedef struct {
} printbuf;

int vsprintf(char* buf, const char* fmt, va_list);
int sprintf(char* buf, const char* fmt, ...);

int printf(const char* fmt, ...);

#endif
