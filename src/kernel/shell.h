#ifndef SHELL_H
#define SHELL_H
#include "arena.h"
#include "ui.h"
#include "../stdlib/stdbool.h"

struct argument_ctx {
    char* args[30];
    int arg_counter;
};

struct builtin_result {
    bool is_clear_command, is_empty_command;
    int code;
    char* string_results[10];
    int string_result_count;
};
#define BUILTIN_RESULT_ERR(_code, _str) \
    (struct builtin_result) {.code=_code,.string_results[0]=_str}
#define BUILTIN_RESULT_SUC_NO_MSG(_code) \
    (struct builtin_result) {.code=_code,.string_results = {0},.string_result_count=1}
#define BUILTIN_RESULT_SUC(_code, _str) \
    (struct builtin_result) {.code=_code,.string_results[0]=_str,.string_result_count=1}
#define BUILTIN_RESULT_SUC_LST(_code, _list, _count) \
    (struct builtin_result) {.code=_code,.string_results=_list,.string_result_count=_count}

int shell_keyboard_listener(char, uint8_t);
int shell_timer_listener(int);

int shell_print_result(char*);
int shell_run(arena*, ui_box_t*);
struct builtin_result shell_process(char*);

struct builtin_result shell_read_builtin(const struct argument_ctx*);
struct builtin_result shell_list_builtin(const struct argument_ctx*);
struct builtin_result shell_newf_builtin(const struct argument_ctx*);
struct builtin_result shell_delf_builtin(const struct argument_ctx*);
struct builtin_result shell_appf_builtin(const struct argument_ctx*);

#endif
