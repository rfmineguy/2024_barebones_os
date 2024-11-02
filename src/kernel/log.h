#ifndef LOG_H
#define LOG_H
#include "serial.h"

#define log_info(cat, fmt, ...) serial_printf("[" cat "][INFO] : " fmt, ##__VA_ARGS__)
#define log_warn(cat, fmt, ...) serial_printf("[" cat "][WARN] : " fmt, ##__VA_ARGS__)
#define log_err(cat, fmt, ...) serial_printf("[" cat "][ERR ] : " fmt, ##__VA_ARGS__)
#define log_crit(cat, fmt, ...) serial_printf("[" cat "][CRIT] : " fmt, ##__VA_ARGS__)

#endif
