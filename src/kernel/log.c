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

    serial_printf("%*s[%s]\n", (group_level * 4), "", log_buf);
    group_level++;
}
void log_group_end(const char* name, ...){
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    group_level--;
    serial_printf("%*s[%s]\n", (group_level * 4), "", log_buf);
}
void log_line_begin(const char* name, ...) {
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    line_length = 0;
    // for (int i = 0; i < group_level * 4; i++) serial_printf(" ");
    serial_printf("%*s[%s] {\n", (group_level * 4), "", log_buf);
}
void log_line_end(const char* name, ...) {
    va_list args;
    va_start(args, name);
    k_vsprintf(log_buf, name, args);
    va_end(args);

    serial_printf("\n");
    serial_printf("%*s} [%s]\n", (group_level * 4), "", log_buf);
}

/* "[%s] %s", cat
 * 
 */
void log_info_internal(const char* type, const char* cat, const char* fmt, ...) {
    //1. format the print into a buffer
    static char buf[1000] = {0};
    static char buf2[1000] = {0};
    va_list args;
    va_start(args, fmt);
    k_vsprintf(buf, fmt, args);
    va_end(args);

    serial_printf("%*s[%s]%s\n", (group_level * 4), "", cat, buf);
}

void log_line_internal(const char* fmt, ...) {
    static char buf[1000] = {0};
    va_list args;
    va_start(args, fmt);
    k_vsprintf(buf, fmt, args);
    va_end(args); 

    int len = strlen(buf);
    if (line_length >= 50) {
        serial_printf("\n");
        line_length = 0;
    }
    if (line_length == 0) serial_printf("%*s", ((group_level + 1) * 4), "");
    line_length += serial_printf("%s ", buf);
    // for (int i = 0; i < len; i++) {
    //     // if (line_length >= 50)
    //     //     serial_printf("\n");
    // }
    // serial_printf("\n");
}
