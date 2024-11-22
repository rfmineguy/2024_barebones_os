#include "printf.h"
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <printf.h>

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

// https://cplusplus.com/reference/cstdio/printf/
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

int powi(int base, int exp) {
    int v = 1;
    for (int i = 0; i < exp; i++)
        v *= base;
    return v;
}
int parse_int(const char* s, int* out_length) {
    static char buf[20];
    // 1. get length of integer
    const char* cursor = s;
    int length = 0;
    while (*cursor && isdigit(*cursor)) {
        length++;
        cursor++;
    }

    int value = 0;
    // 2. parse the characters into an integer
    for (int i = 0; i < length; i++) {
        int exponent = (length - i - 1);
        int digit = s[i] - '0';
        value += (int)powi(10, exponent) * digit;
    }
    if (out_length) *out_length = length;
    return value;
}

// %[flags][width][.precision][length]specifier
// ("Hello %s", "World")
int fmt_spec_parse(const char* fmt, fmt_spec* out_spec) {
    if (!out_spec) return -1;
    if (*fmt != '%') return -1;
    const char* cursor = fmt;
    cursor++;
    while (*cursor) {
        if (*cursor == '-') out_spec->flags |= 0x1; 
        else if (*cursor == '+') out_spec->flags |= 0x2;
        else if (*cursor == '0') out_spec->flags |= 0x4;
        else if (*cursor == ' ') out_spec->flags |= 0x8;
        else if (*cursor == '#') out_spec->flags |= 0x10;
        else break;
        cursor++;
    }

    // parse width
    if (*cursor == '*') {
        out_spec->width = -1;
        cursor++;
    }
    else if (isdigit(*cursor)) {
        int length = 0;
        out_spec->width = parse_int(cursor, &length);
        cursor += length;
    }
    else {
        // default width
    }

    // parse precision
    if (*cursor == '.') {
        cursor++;
        if (*cursor == '*') {
            out_spec->precision = -1;
            cursor++;
        }
        else if (isdigit(*cursor)) {
            int length = 0;
            out_spec->precision = parse_int(cursor, &length);
            cursor += length;
        }
    }

    // parse length (bit length of value) (a character)
    out_spec->length = '.'; // no length
    if (*cursor == 'h' || *cursor == 'l' || *cursor == 'L') {
        out_spec->length = *cursor;
        cursor++;
    }

    // parse the specifier
    if (*cursor == 'd' || *cursor == 'i' || *cursor == 'u' || *cursor == 'o' ||
        *cursor == 'x' || *cursor == 'X' || *cursor == 'c' || *cursor == 's' ||
        *cursor == 'p' || *cursor == '%') {
        out_spec->specifier = *cursor;
        cursor++;
    }
    return cursor - fmt;
}

void debug_fmt_spec(fmt_spec s) {
    printf("=================\n");
    printf("flags=%d\n", s.flags);
    printf("width=%d\n", s.width);
    printf("length=%c\n", s.length);
    printf("precision=%d\n", s.precision);
    printf("specifier=%c\n", s.specifier);
    printf("=================\n");
}

int stringify_base(char buf[20], int value, int base, bool uppercase){
    static char internal_buf[20];
    int digit_i = 0;
    if (value == 0) internal_buf[digit_i++] = '0';
    while (value != 0) {
        int v = value % base;
        internal_buf[digit_i++] = uppercase ?
            toupper(base_chars[v]) : base_chars[v];
        value /= base;
    }
    for (int i = digit_i - 1; i >= 0; i--) {
        buf[digit_i - 1 - i] = internal_buf[i];
    }
    return digit_i;
}

int k_vsnprintf2(char* buf, int n, const char* fmt, va_list list) {
#define min(a, b) (a) < (b) ? (a) : (b)
#define max(a, b) (a) >= (b) ? (a) : (b)
    const char* cursor = fmt;
    int state = NORMAL;
    fmt_spec spec;
    while (*cursor) {
        if (state == NORMAL && *cursor == '%') {
            state = PARSE;
            // cursor++;
            continue;
        }
        switch (state) {
            case NORMAL: buf = sprint_ch(buf, *(cursor++)); 
                         continue;
            case PARSE: {
                 int fmt_len = 0;
                 spec = (fmt_spec){};
                 if ((fmt_len = fmt_spec_parse(cursor, &spec)) != 0) {
                     debug_fmt_spec(spec);
                    if (spec.flags & 0x1) {} // left justify
                    if (spec.flags & 0x2) {} // signed print
                    if (spec.flags & 0x4) {} // space print before value
                    if (spec.flags & 0x8) {} //
                    if (spec.flags & 0x10) {} // left padding with 0s

                    if ('%' == spec.specifier) {
                        buf = sprint_ch(buf, '%');
                    }
                    if ('c' == spec.specifier) {
                        if (spec.width == -1) spec.width = va_arg(list, int);
                        for (int i = 0; i < spec.width - 1; i++)
                            buf = sprint_ch(buf, ' ');
                        buf = sprint_ch(buf, va_arg(list, int));
                    }
                    if ('s' == spec.specifier) {
                        if (spec.width == -1)     spec.width = va_arg(list, int);
                        if (spec.precision == -1) spec.precision = va_arg(list, int);
                        const char* arg = va_arg(list, const char*);
                        if (spec.width == 0 && spec.precision == 0) {
                            while (*arg) {
                                buf = sprint_ch(buf, *arg);
                                arg++;
                            }
                        }
                        else {
                            int to_print = min(spec.width, strlen(arg));
                            int space_to_print = spec.width - to_print;
                            for (int i = 0; i < space_to_print; i++) {
                                buf = sprint_ch(buf, ' ');
                            }
                            for (int i = 0; i < to_print; i++) {
                                buf = sprint_ch(buf, arg[i]);
                            }
                        }
                    }
                    if ('d' == spec.specifier || 'i' == spec.specifier) {
                        char number_buf[20];
                        if (spec.width == -1)     spec.width = va_arg(list, int);
                        if (spec.precision == -1) spec.precision = va_arg(list, int);
                        int arg = va_arg(list, int);
                        int length = stringify_base(number_buf, arg, 10, false);
                        int space_to_print = spec.width - length;
                        for (int i = 0; i < space_to_print; i++) {
                            buf = sprint_ch(buf, ' ');
                        }
                        for (int i = 0; i < length; i++) {
                            buf = sprint_ch(buf, number_buf[i]);
                        }
                    }
                    if ('x' == spec.specifier || 'X' == spec.specifier) {
                        int uppercase = spec.specifier == 'X';
                        char number_buf[20];
                        if (spec.width == -1)     spec.width = va_arg(list, int);
                        if (spec.precision == -1) spec.precision = va_arg(list, int);
                        int arg = va_arg(list, int);
                        int length = stringify_base(number_buf, arg, 16, uppercase);
                        int space_to_print = spec.width - length;
                        for (int i = 0; i < space_to_print; i++) {
                            buf = sprint_ch(buf, ' ');
                        }
                        for (int i = 0; i < length; i++) {
                            buf = sprint_ch(buf, number_buf[i]);
                        }
                    }
                    if ('p' == spec.specifier) {
                        int uppercase = spec.specifier == 'X';
                        char number_buf[20];
                        if (spec.width == -1)     spec.width = va_arg(list, int);
                        if (spec.precision == -1) spec.precision = va_arg(list, int);
                        int arg = va_arg(list, int);
                        int length = stringify_base(number_buf, arg, 16, uppercase);
                        int space_to_print = spec.width - length - 2;
                        for (int i = 0; i < space_to_print; i++) {
                            buf = sprint_ch(buf, ' ');
                        }
                        buf = sprint_str(buf, "0x");
                        for (int i = 0; i < length; i++) {
                            buf = sprint_ch(buf, number_buf[i]);
                        }
                    }
                 }
                 cursor += fmt_len;
                 state = NORMAL;
                 break;
            }
        }
    }
    *buf = 0;
    return 0;
}

int k_sprintf(char* buf, const char* fmt, ...) {
    va_list alist;
    va_start(alist, fmt);
    int x = k_vsprintf(buf, fmt, alist);
    va_end(alist);
    return x;
}

int k_printf2(const char* fmt, ...) {
    static char buf[1000] = {0};
    va_list alist;
    va_start(alist, fmt);
    int x = k_vsnprintf2(buf, 1000, fmt, alist);
    va_end(alist);

    printf("%s", buf);
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
    // vga_writestring(buf);
    return x;
}
#endif
