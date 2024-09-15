#ifndef LOG_H
#define LOG_H

#define log_info(tag, fmt, ...) log(0, tag, fmt, __VA_ARGS__)
#define log_warn(tag, fmt, ...) log(0, tag, fmt, __VA_ARGS__)
#define log_err (tag, fmt, ...) log(0, tag, fmt, __VA_ARGS__)
#define log_crit(tag, fmt, ...) log(0, tag, fmt, __VA_ARGS__)

void log(int level, const char* tag, const char* fmt, ...);

#endif
