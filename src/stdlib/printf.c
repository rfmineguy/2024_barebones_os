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

int printf(const char* fmt, ...) {
    const char* cursor = fmt;
    int state = 0;
    int chars_printed = 0;
    va_list alist;
    va_start(alist, fmt);
    while (*cursor) {
        if (state == NORMAL) {
            if (*cursor == '%') state = PARSE;
            else vga_putch(*cursor);
        }
        else if (state == PARSE) {
            switch (*cursor) {
                case '%': chars_printed++; vga_putch('%'); break;
                case 'c': chars_printed++; vga_putch(va_arg(alist, int)); break;
                case 'd': chars_printed += print_base(va_arg(alist, int), 10); break;
                case 'x': chars_printed += print_base(va_arg(alist, int), 16); break;
                default: break;
            }
            state = NORMAL;
        }
        cursor++;
    }
    
    va_end(alist);
    return chars_printed;
}
