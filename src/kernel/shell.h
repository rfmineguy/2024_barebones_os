#ifndef SHELL_H
#define SHELL_H

struct argument_ctx {
    const char* args[30];
    int arg_counter;
};

int shell_keyboard_listener(char);
int shell_run();
int shell_process(const char*);

int shell_read_builtin(const struct argument_ctx*);

#endif
