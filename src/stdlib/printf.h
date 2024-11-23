#ifndef PRINTF_H
#define PRINTF_H
#include "../kernel/vga.h"
#include "stdarg.h"

typedef struct fmt_spec {
    uint8_t flags;
    int width;
    char length;
    int precision;
    char specifier;
} fmt_spec;

int k_vsprintf(char* buf, const char* fmt, va_list);
int k_sprintf(char* buf, const char* fmt, ...);

int k_printf(const char* fmt, ...);
int k_vprintf(const char* fmt, va_list);

#endif
