#include "printf.h"
#include <stdarg.h>
#include <limits.h>

#define NORMAL 0
#define PARSE  1

/*  652
 *  600 + 50 + 2
 */
int print_integer(int i) {
  char buf[20] = {0};
  int digit = 0;
  int valcp = i;
  
  while (valcp != 0) {
    buf[digit] = valcp % 10 + '0';
    digit++;
    valcp /= 10;
  }
  
  for (int i = digit - 1; i >= 0; i--) {
    vga_putch(buf[i]);
  }
  return digit;
}

char* base_chars = "012345679abcdef";
int print_base(int i, int base) {
    char buf[20] = {0};
    int digit = 0;

    if (i == 0) buf[digit++] = '0';
    while (i != 0) {
        int v = i % base;
        buf[digit++] = base_chars[v];
        i /= base;
    }
    for (int i = digit - 1; i >= 0; i--) {
      vga_putch(buf[i]);
    }
    return digit;
}

char* sprint_ch(char* buf, char c) {
    *buf = c;
    return buf + 1;
}

char* sprint_base(char* buf, int i, int base) {
    const char* bufSave = buf;
    char buf2[20] = {0};
    int digit = 0;

    if (i == 0) buf2[digit++] = '0';
    while (i != 0) {
        int v = i % base;
        buf2[digit++] = base_chars[v];
        i /= base;
    }
    for (int i = digit - 1; i >= 0; i--) {
        *(buf++) = buf2[i];
    }
    return buf;
}

char* sprint_str(char* buf, const char* str) {
    while (*str) {
        *(buf++) = *str;
        str++;
    }
    return buf;
}

int vsprintf(char* buf, const char* fmt, va_list alist) {
    const char* cursor = fmt;
    char *buf_orig = buf;
    int state = 0;
    while (*cursor) {
        if (state == NORMAL) {
            if (*cursor == '%') state = PARSE;
            else buf = sprint_ch(buf, *cursor);
        }
        else if (state == PARSE) {
            switch (*cursor) {
                case '%': buf = sprint_ch(buf, '%'); break;
                case 'c': buf = sprint_ch(buf, '%'); break;
                case 's': buf = sprint_str(buf, va_arg(alist, const char*)); break;
                case 'x': {
                          unsigned int v = va_arg(alist, unsigned int);
                          buf = sprint_base(buf, v, 16); break;
                          }
                case 'd': {
                          unsigned int v = va_arg(alist, unsigned int);
                          buf = sprint_base(buf, v, 10); break;
                          }
                case 'b': {
                          unsigned int v = va_arg(alist, unsigned int);
                          buf = sprint_base(buf, v, 2); break;
                          }
                default: break;
            }
            state = NORMAL;
        }
        cursor++;
    }
    
    return buf - buf_orig;
}

int sprintf(char* buf, const char* fmt, ...) {
    va_list alist;
    va_start(alist, fmt);
    int x = vsprintf(buf, fmt, alist);
    va_end(alist);
    return x;
}

int printf(const char* fmt, ...) {
    static char buf[1000] = {0};
    va_list alist;
    va_start(alist, fmt);
    int x = vsprintf(buf, fmt, alist);
    va_end(alist);
    buf[x] = 0;
    vga_writestring(buf);
    return x;
}
