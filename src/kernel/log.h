#ifndef LOG_H
#define LOG_H
#include "serial.h"

extern int group_level;
void log_info_internal(const char*, const char*, const char*, ...);
void log_line_internal(const char*, ...);

void log_group_begin_internal(const char*, ...);
void log_group_end_internal(const char*, ...);

void log_line_begin_internal(const char*, ...);
void log_line_end_internal(const char*, ...);
 
#ifndef LOG_DISABLE
#define log_none(type, cat, fmt, ...) \
    log_info_internal(type, cat, fmt, ##__VA_ARGS__)
#define log_line(fmt, ...) do { log_line_internal(fmt, ##__VA_ARGS__); } while(0)
#define log_info(cat, fmt, ...) do { log_none("INFO", cat, fmt, ##__VA_ARGS__); } while(0)
#define log_warn(cat, fmt, ...) do { log_none("WARN", cat, fmt, ##__VA_ARGS__); } while(0)
#define log_err(cat, fmt, ...) do { log_none("ERRR", cat, fmt, ##__VA_ARGS__); } while(0)
#define log_crit(cat, fmt, ...) do { log_none("CRIT", cat, fmt, ##__VA_ARGS__); } while(0)
#define log_group_begin(fmt, ...) do { log_group_begin_internal(fmt, ##__VA_ARGS__); } while(0)
#define log_group_end(fmt, ...) do { log_group_end_internal(fmt, ##__VA_ARGS__); } while(0)
#define log_line_begin(fmt, ...) do { log_line_begin_internal(fmt, ##__VA_ARGS__); } while(0)
#define log_line_end(fmt, ...) do { log_line_end_internal(fmt, ##__VA_ARGS__); } while(0)
#else
#define log_line(fmt, ...)
#define log_info(cat, fmt, ...)
#define log_warn(cat, fmt, ...)
#define log_err(cat, fmt, ...)
#define log_crit(cat, fmt, ...)
#define log_group_begin(fmt, ...)
#define log_group_end(fmt, ...)
#define log_line_begin(fmt, ...)
#define log_line_end(fmt, ...)
#endif

#endif
