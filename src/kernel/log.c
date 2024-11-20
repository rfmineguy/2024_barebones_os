#include "log.h"
#include "../stdlib/stdarg.h"
#include "../stdlib/printf.h"
#include "../stdlib/string.h"

int group_level = 0;
int line_length = 0; // used for line groups
int lines = 0; // used for line groups

char log_buf[500];

void log_group_begin(const char* name, ...){
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    for (int i = 0; i < group_level * 4; i++) serial_printf(" ");
    serial_printf("[%s]\n", log_buf);
    group_level++;
}
void log_group_end(const char* name, ...){
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    group_level--;
    for (int i = 0; i < group_level * 4; i++) serial_printf(" ");
    serial_printf("[%s]\n", log_buf);
}
void log_line_begin(const char* name, ...) {
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    line_length = 0;
    for (int i = 0; i < group_level * 4; i++) serial_printf(" ");
    serial_printf("[%s] {\n", log_buf);
}
void log_line_end(const char* name, ...) {
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    serial_printf("\n");
    for (int i = 0; i < group_level * 4; i++) serial_printf(" ");
    serial_printf("} [%s]\n", log_buf);
}
void log_info_internal(const char* type, const char* cat, const char* fmt, ...) {
    //1. format the print into a buffer
    static char buf[1000] = {0};
    va_list args;
    va_start(args, fmt);
    k_vsprintf(buf, fmt, args);
    va_end(args);

    //2. format the cat into a buffer
    static char buf2[250] = {0};
    k_sprintf(buf2, "[%s]  ", cat);

    //2. print the formatted string on multiple lines (max 50 character line limit)
    int len = strlen(buf);
    int lines = len / 50;
    int index = 0;
    for (int i = 0; i <= lines; i++) {
        for (int ix = 0; ix < group_level * 4; ix++) serial_printf(" ");
        serial_write_str(buf2);
        while (index < len && buf[index]) {
            serial_write_ch(buf[index++]);
            if (i != 0 && index % 50 == 0) break;
        }
        serial_write_ch('\n');
    }
}
void log_line_internal(const char* fmt, ...) {
    static char buf[1000] = {0};
    va_list args;
    va_start(args, fmt);
    k_vsprintf(buf, fmt, args);
    va_end(args);

    int len = strlen(buf);
    int index = 0;
    while (index < len && buf[index]) {
        if (line_length % 50 == 0)
            for (int i = 0; i < (group_level + 1) * 4; i++) serial_printf(" ");
        serial_write_ch(buf[index++]);
        line_length++;
        if (line_length % 50 == 0) {
            serial_write_ch('\n');
            lines++;
        }
    }
}
