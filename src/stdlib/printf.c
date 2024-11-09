#include "printf.h"
#include "stdarg.h"
#include "stdbool.h"
#include "ctype.h"

#define NORMAL 0
#define PARSE  1

char* base_chars = "0123456789abcdef";
char* sprint_base(char* buf, uint32_t i, int base, bool uppercase) {
    char buf2[20] = {0};
    int digit = 0;

    if (i == 0) buf2[digit++] = '0';
    while (i != 0) {
        int v = i % base;
        buf2[digit++] = uppercase ?
            toupper(base_chars[v]) :
            base_chars[v];
        i /= base;
    }
    for (int i = digit - 1; i >= 0; i--) {
        *(buf++) = buf2[i];
    }
    return buf;
}

char* sprint_ch(char* buf, char c) {
    *buf = c;
    return buf + 1;
}

char* sprint_ch_n(char* buf, char c, int n) {
    for (int i = 0; i < n; i++) {
        buf[i] = c;
    }
    return buf + n;
}

char* sprint_str(char* buf, const char* str) {
    while (*str) {
        *(buf++) = *str;
        str++;
    }
    return buf;
}

int k_vsprintf(char* buf, const char* fmt, va_list alist) {
    const char* cursor = fmt;
    char *buf_orig = buf;
    int state = 0;
    int number = 0;
    char c = ' ';
    while (*cursor) {
        if (state == NORMAL) {
            number = 0;
            if (*cursor == '%') state = PARSE;
            else buf = sprint_ch(buf, *cursor);
        }
        else if (state == PARSE) {
            while (isdigit(*cursor)) {
                number = number * 10 + (*cursor - '0');
                cursor++;
            }
            switch (*cursor) {
                case '%': buf = sprint_ch(buf, '%'); break;
                case 'c': {
                              c = va_arg(alist, unsigned char);
                              if (number != 10000) {
                                buf = sprint_ch_n(buf, c, number);
                              } else {
                                buf = sprint_ch(buf, c);
                              }
                              break;
                          }
                case 's': buf = sprint_str(buf, va_arg(alist, const char*)); break;
                case 'X': {
                          uint32_t v = va_arg(alist, uint32_t);
                          buf = sprint_base(buf, v, 16, true); break;
                          }
                case 'x': {
                          uint32_t v = va_arg(alist, uint32_t);
                          buf = sprint_base(buf, v, 16, false); break;
                          }
                case 'd': {
                          uint32_t v = va_arg(alist, uint32_t);
                          buf = sprint_base(buf, v, 10, false); break;
                          }
                case 'b': {
                          uint32_t v = va_arg(alist, uint32_t);
                          buf = sprint_base(buf, v, 2, false); break;
                          }
                default: break;
            }
            state = NORMAL;
        }
        cursor++;
    }
    *buf = 0;
    buf++;
    
    return buf - buf_orig;
}

int k_sprintf(char* buf, const char* fmt, ...) {
    va_list alist;
    va_start(alist, fmt);
    int x = k_vsprintf(buf, fmt, alist);
    va_end(alist);
    return x;
}

// Dont compile this function if -DTESTING
#ifndef TESTING
int k_printf(const char* fmt, ...) {
    va_list alist;
    va_start(alist, fmt);
    int x = k_vprintf(fmt, alist);
    va_end(alist);
    return x;
}

int k_vprintf(const char* fmt, va_list alist) {
    static char buf[1000] = {0};
    int x = k_vsprintf(buf, fmt, alist);
    vga_writestring(buf);
    return x;
}
#endif
