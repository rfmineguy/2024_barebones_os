#ifndef LOG_H
#define LOG_H
#include "serial.h"

extern int group_level;
extern void log_info_internal(const char*, const char*, const char*, ...);
extern void log_line_internal(const char*, ...);

void log_group_begin(const char*, ...);
void log_group_end(const char*, ...);

void log_line_begin(const char*, ...);
void log_line_end(const char*, ...);

// #define log_line_begin(x) log_group_begin(x)
// #define log_line_end(x) log_group_end(x)

#define log_none(type, cat, fmt, ...) \
    log_info_internal(type, cat, fmt, ##__VA_ARGS__)

#define LOG_ENABLE
#ifdef  LOG_ENABLE
#define log_line(fmt, ...) do { log_line_internal(fmt, ##__VA_ARGS__); } while(0);
#define log_info(cat, fmt, ...) do { log_none("INFO", cat, fmt, ##__VA_ARGS__); } while(0);
#define log_warn(cat, fmt, ...) do { log_none("WARN", cat, fmt, ##__VA_ARGS__); } while(0);
#define log_err(cat, fmt, ...) do { log_none("ERRR", cat, fmt, ##__VA_ARGS__); } while(0);
#define log_crit(cat, fmt, ...) do { log_none("CRIT", cat, fmt, ##__VA_ARGS__); } while(0);
#else
#define log_line(fmt, ...)
#define log_info(cat, fmt, ...)
#define log_warn(cat, fmt, ...)
#define log_err(cat, fmt, ...)
#define log_crit(cat, fmt, ...)
#endif

#endif
