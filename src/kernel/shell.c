#include "shell.h"
#include "log.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/string.h"
#include "../stdlib/ctype.h"
#include "../stdlib/printf.h"
#include "vga.h"
#include "keyboard.h"
#include "error.h"
#include "fat.h"
#include "arena.h"

bool return_pressed = false;
char shell_buffer[1024] = {0};
int shell_buffer_i = 0;
arena* kernel_arena = (void*)0;

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
int shell_run(arena* _kernel_arena) {
    kernel_arena = _kernel_arena;
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
            if (ret > 0) {
                log_err("ShellRun", "%s\n", map_error_code(ret));
                k_printf("%s\n", map_error_code(ret));
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
    if (strcmp(ctx.args[0], "dir") == 0)  return shell_dir_builtin(&ctx);

    return ERROR_INVALID_CMD; // invalid command supplied
}

// shell command
//    read <filename>
//    
//    <filename> should be converted to 8.3 format for fat12
//      i.e.   "file.txt" -> "FILE    TXT" 
// return:
//   0 = success
//   1 = file not found
//   2 = failed to read
int shell_read_builtin(const struct argument_ctx* arg_ctx) {
    log_info("ShellBuiltin", "Read file\n");
    char name_8_3[12] = "           ";
    if (fat_filename_to_8_3(arg_ctx->args[1], name_8_3) == -1) {
        k_printf("  !!Error %d\n");
        return ERROR_GENERIC;
    }
    log_info("ShellBuiltin", "%s\n", name_8_3);
    dir_entry* f = (void*)0;
    if (!(f = fat_find_file(name_8_3))) {
       return ERROR_FILE_NOT_FOUND;
    }

    uint8_t* buf = fat_read_entry(f, kernel_arena);
    if (!buf) log_info("ShellBuiltin", "Failed to read file\n");
    else {
        for (size_t i = 0; i < f->Size; i++) {
            if (isprint(buf[i])) {
                serial_printf("%c", buf[i]);
                k_printf("%c", buf[i]);
            }
            else {
               serial_printf("<%X>", buf[i]);
               k_printf("<%X>", buf[i]);
            }
        }
        k_printf("\n");
    }

    return ERROR_NONE;
}

int shell_dir_builtin(const struct argument_ctx* arg_ctx) {
    (void)arg_ctx;
    log_info("ShellBuiltin", "Dir\n");
    return ERROR_NONE;
}
