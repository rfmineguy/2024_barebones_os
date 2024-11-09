#ifndef ERROR_H
#define ERROR_H

#define ERROR_NONE 0
#define ERROR_GENERIC 1
#define ERROR_INVALID_CMD 2
#define ERROR_FILE_NOT_FOUND 3
#define ERROR_FILE_READ 4
#define ERROR_INVALID 5

char* map_error_code(int);

#endif
