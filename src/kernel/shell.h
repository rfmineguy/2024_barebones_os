#ifndef SHELL_H
#define SHELL_H

struct argument_ctx {
    char* args[30];
    int arg_counter;
};

int shell_keyboard_listener(char);
int shell_run();
int shell_process(const char*);

int shell_read_builtin(const struct argument_ctx*);
int shell_dir_builtin(const struct argument_ctx*);

#endif
