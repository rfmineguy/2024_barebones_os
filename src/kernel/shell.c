#include "shell.h"
#include "log.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/string.h"
#include "vga.h"
#include "keyboard.h"

bool return_pressed = false;
char shell_buffer[1024] = {0};
int shell_buffer_i = 0;

int shell_keyboard_listener(char ch) {
   return_pressed = false;
   switch (ch) {
       case '\n': // log_info("ShellKbd", "Return key\n");
                  return_pressed = true;
                  break;
       case 8:    // log_info("ShellKbd", "Delete key\n");
                  if (shell_buffer_i <= 0) {
                      return ch;
                  }
                  shell_buffer_i--;
                  shell_buffer[shell_buffer_i] = 0;
                  break;
       default:   // log_info("ShellKbd", "Got key %d\n", ch);
                  if (shell_buffer_i > 1024) {
                    log_crit("ShellKbd", "Shell buffer full");
                    break;
                  }
                  shell_buffer[shell_buffer_i++] = ch;
                  shell_buffer[shell_buffer_i] = 0; // always null terminate buffer
                  break;
   }

   return ch;
}

// shell runs in kernel level. just for simplicity
int shell_run() {
    int exit_code = 0;
    while (1) {
        while (!return_pressed) {
            vga_clearline();
            size_t x, y;
            vga_save_cursor(&x, &y);
            vga_writestring("> ");
            vga_write(shell_buffer, 80);
            vga_put_cursor_at(x, y);
        }
        if (return_pressed) {
            return_pressed = false;
            vga_putch('\n');
            // process command
            log_info("ShellRun", "Processing '%s'\n", shell_buffer);
            int ret = shell_process(shell_buffer);
            if (ret == -1) {
                // invalid command
            }

            // clear buffer
            shell_buffer_i = 0;
            shell_buffer[0] = 0;
            for (int i = 0; i < 80; i++) shell_buffer[i] = 0;
        }
    }
    return exit_code;
}

void tokenize_args(char* str, struct argument_ctx* ctx) {
    ctx->arg_counter = 0;

    const char* start = str;
    while (*str != 0) {
        if (*str == ' ') {
            *str = 0;
            ctx->args[ctx->arg_counter++] = start;
            start = str + 1;
        }
        str++;
    }
    ctx->args[ctx->arg_counter++] = start;
}

// shell command format
//    <command> <args(space separated)>
int shell_process(const char* buf) {
    struct argument_ctx ctx = {0};
    tokenize_args(buf, &ctx);

    for (int i = 0; i < ctx.arg_counter; i++) {
        log_info("ShellProc", "Arg #%d = %s\n", i, ctx.args[i]);
    }
    if (strcmp(ctx.args[0], "read") == 0) return shell_read_builtin(&ctx);

    return -1; // invalid command supplied
}

// shell command
//    read <filename>
// return:
//   0 = success
//   1 = file not found
//   2 = failed to read
int shell_read_builtin(const struct argument_ctx* arg_ctx) {
    log_info("ShellBuiltin", "Read file\n");
}
