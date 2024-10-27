#ifndef PRINTF_H
#define PRINTF_H
#include "../kernel/vga.h"
#include "stdarg.h"

int k_vsprintf(char* buf, const char* fmt, va_list);
int k_sprintf(char* buf, const char* fmt, ...);

int k_printf(const char* fmt, ...);

#endif
