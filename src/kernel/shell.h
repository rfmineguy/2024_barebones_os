#ifndef SHELL_H
#define SHELL_H

int shell_keyboard_listener(char);
int shell_run();
int shell_process(const char*);

int shell_read_builtin(const char*);

#endif
