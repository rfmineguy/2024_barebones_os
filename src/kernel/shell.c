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
#include "ui.h"

bool return_pressed = false, backspace_pressed = false;
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
                  backspace_pressed = true;
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
int shell_run(arena* _kernel_arena, ui_box* box) {
    kernel_arena = _kernel_arena;
    int exit_code = 0;
    struct builtin_result r;
    ui_box_clear(box);

    while (1) {
        ui_box_draw(box);

        // perform some logic when enter is pressed
        if (return_pressed) {
            return_pressed = false;
            box->cursorx = 0;
            box->cursory++;
            ui_box_clearregion(box, 1, box->cursory, 3 + strlen(shell_buffer), 1);

            // process command
            log_info("ShellRun", "Processing '%s'\n", shell_buffer);
            r = shell_process(shell_buffer);
            if (r.code > 0) {
                log_err("ShellRun", "%s\n", map_error_code(r.code));
                ui_box_printf(box, box->cursorx, box->cursory, "!!%s\n", map_error_code(r.code));
            }
            else {
                log_info("ShellRun", "Command ran successfully\n");
                if (r.string_result)
                    ui_box_printf(box, box->cursorx, box->cursory, "%s\n", r.string_result);
            }
            box->cursory++;
            if (box->cursory >= box->y + box->h) box->cursory = 1;

            // clear buffer
            shell_buffer_i = 0;
            shell_buffer[0] = 0;
            for (int i = 0; i < 80; i++) shell_buffer[i] = 0;
        }

        // display state of current shell line
        while (!return_pressed) {
            // scroll buffer
            if (box->cursory >= box->h - 4) {
                // ui_box_scroll_vertical(box);
                ui_box_scroll_vertical(box);
                box->cursory-=1;
            }
            size_t x, y;
            if (backspace_pressed) {
                ui_box_clearregion(box, 1, box->cursory, 3 + strlen(shell_buffer), 1);
                backspace_pressed = false;
            }
            ui_box_printf(box, 0, box->cursory, "> ");
            ui_box_printf(box, box->cursorx, box->cursory, "%s", shell_buffer);
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
struct builtin_result shell_process(const char* buf) {
    struct argument_ctx ctx = {0};
    tokenize_args(buf, &ctx);

    for (int i = 0; i < ctx.arg_counter; i++) {
        log_info("ShellProc", "Arg #%d = %s\n", i, ctx.args[i]);
    }
    if (strcmp(ctx.args[0], "read") == 0) return shell_read_builtin(&ctx);
    if (strcmp(ctx.args[0], "dir") == 0)  return shell_dir_builtin(&ctx);

    return BUILTIN_RESULT(ERROR_INVALID_CMD, (void*)0); // invalid command supplied
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
struct builtin_result shell_read_builtin(const struct argument_ctx* arg_ctx) {
    log_info("ShellBuiltin", "Read file\n");
    char name_8_3[12] = "           ";
    if (fat_filename_to_8_3(arg_ctx->args[1], name_8_3) == -1) {
        return BUILTIN_RESULT(ERROR_GENERIC, (void*)0);
    }
    log_info("ShellBuiltin", "%s\n", name_8_3);
    dir_entry* f = (void*)0;
    if (!(f = fat_find_file(name_8_3))) {
        return BUILTIN_RESULT(ERROR_FILE_NOT_FOUND, (void*)0);
    }

    uint8_t* buf = fat_read_entry(f, kernel_arena);
    if (!buf) {
        log_info("ShellBuiltin", "Failed to read file\n");
        return BUILTIN_RESULT(ERROR_FILE_READ, (void*)0);
    }
    
    else {
        return BUILTIN_RESULT(ERROR_NONE, (char*)buf);
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

    return BUILTIN_RESULT(ERROR_GENERIC, (void*)0);
}

struct builtin_result shell_dir_builtin(const struct argument_ctx* arg_ctx) {
    (void)arg_ctx;
    log_info("ShellBuiltin", "Dir\n");
    return BUILTIN_RESULT(ERROR_NONE, (void*)0);
}
