#include "error.h"

#define ERROR_MAP_SIZE 20

char* error_to_str_map[ERROR_MAP_SIZE] = {
    [0]  = "No error",
    [1]  = "Generic error",
    [2]  = "Invalid shell command",
    [3]  = "File not found",
    [4]  = "File read failed",
    [5]  = "Unimplemented command",
    [6]  = "File exists",
    [7]  = "File too big",
    [8]  = "File already exists",
    [9]  = "Malformed filename",
    [10] = "No available cluster",
    [11] = "Invalid", 
    [12] = "Invalid",
    [13] = "Invalid",
    [14] = "Invalid", 
    [15] = "Invalid", 
    [16] = "Invalid", 
    [17] = "Invalid", 
    [18] = "Invalid", 
    [19] = "Invalid",
};

char* map_error_code(int code) {
    if (code < 0 || code >= ERROR_MAP_SIZE) return "Out of bounds error code";
    return error_to_str_map[code];
}
