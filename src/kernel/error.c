#include "error.h"

#define ERROR_MAP_SIZE 10

char* error_to_str_map[ERROR_MAP_SIZE] = {
    [0] = "No error",
    [1] = "Generic error",
    [2] = "Invalid shell command",
    [3] = "File not found",
    [4] = "File read failed",
    [5] = "Unimplemented command",
    [6] = "File exists",
    [7] = "Invalid",
    [8] = "Invalid",
    [9] = "Invalid",
};

char* map_error_code(int code) {
    if (code < 0 || code >= ERROR_MAP_SIZE) return "Out of bounds code";
    return error_to_str_map[code];
}
