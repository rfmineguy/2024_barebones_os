#ifndef SHELL_H
#define SHELL_H
#include "arena.h"
#include "ui.h"

struct argument_ctx {
    char* args[30];
    int arg_counter;
};

struct builtin_result {
    int code;
    char* string_result;
};
#define BUILTIN_RESULT(_code, _str) (struct builtin_result) {.code=_code,.string_result=_str}

int shell_keyboard_listener(char);
int shell_timer_listener(int);

int shell_print_result(char*);
int shell_run(arena*, ui_box_t*);
struct builtin_result shell_process(char*);

struct builtin_result shell_read_builtin(const struct argument_ctx*);
struct builtin_result shell_dir_builtin(const struct argument_ctx*);

#endif
