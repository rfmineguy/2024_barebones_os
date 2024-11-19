#include "shell.h"
#include "log.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/string.h"
#include "../stdlib/ctype.h"
#include "../stdlib/printf.h"
#include "vga.h"
#include "keyboard.h"
#include "error.h"
#include "fat_drive.h"
#include "ui.h"
#include "sys.h"

bool cursor_on;
bool up_pressed = false, down_pressed = false;
bool return_pressed = false, backspace_pressed = false, other_pressed = false;
char shell_buffer[1024] = {0};
int shell_buffer_i = 0;
arena* kernel_arena = (void*)0;

int shell_keyboard_listener(char ch) {
   switch (ch) {
       case '\n': // log_info("ShellKbd", "Return key\n");
                  return_pressed = true;
                  break;
       case PGUP: up_pressed = true;
                  break;
       case PGDOWN: down_pressed = true;
                  break;
       case 8:    // log_info("ShellKbd", "Delete key\n");
                  if (shell_buffer_i <= 0) {
                      return ch;
                  }
                  backspace_pressed = true;
                  shell_buffer_i--;
                  shell_buffer[shell_buffer_i] = 0;
                  break;
       case ';':  up_pressed = true;
                  break;
       default:   // log_info("ShellKbd", "Got key %d\n", ch);
                  if (!isprint(ch)) break;
                  if (shell_buffer_i >= 1024) {
                    log_crit("ShellKbd", "Shell buffer full");
                    break;
                  }
                  other_pressed = true;
                  shell_buffer[shell_buffer_i++] = ch;
                  shell_buffer[shell_buffer_i] = 0; // always null terminate buffer
                  break;
   }

   return ch;
}

int shell_timer_listener(int ticks) {
    (void)ticks;
    if (ticks % 20 == 0) {
        cursor_on = !cursor_on;
    }
    return 0;
}

int shell_run(arena* _kernel_arena, ui_box_t* box) {
    (void)_kernel_arena;

    int current_line = 0;
    struct builtin_result r;
    ui_box(box);

    while (1) {
        ui_putch(box, 0, current_line, '>');

        if (return_pressed) {
            current_line++;
            return_pressed = false;
            int output_linecount = 0;

            // process shell buffer
            r = shell_process(shell_buffer);
            shell_buffer_i = 0;
            shell_buffer[0] = 0;

            switch (r.code) {
            case ERROR_NONE:
                if (r.string_result) {
                    output_linecount = ui_putstr(box, 2, current_line, r.string_result);
                }
                break;
            default:
                output_linecount = ui_putstr(box, 2, current_line, map_error_code(r.code));
                break;
            }

            // scroll if needed
            if (current_line + output_linecount >= 8) {
                ui_scroll_vertical_n(box, output_linecount + 1);
                current_line --;
            }
            else {
                current_line += output_linecount;
            }
        }
        if (up_pressed) {
            current_line -= ui_scroll_vertical(box);
            up_pressed = false;
        }
        if (backspace_pressed) {
            ui_clear_rv(box, 2, current_line, 50, 1);
            ui_putstr(box, 2, current_line, shell_buffer);
            backspace_pressed = false;
        }
        if (other_pressed) {
            ui_putstr(box, 2, current_line, shell_buffer);
            other_pressed = false;
        }
        ui_putch(box, 2 + strlen(shell_buffer) + 1, current_line, cursor_on ? '_' : ' ');
        ui_refresh();
    }
    return 0;
}

static void tokenize_args(char* str, struct argument_ctx* ctx) {
    ctx->arg_counter = 0;

    char* start = str;
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
struct builtin_result shell_process(char* buf) {
    struct argument_ctx ctx = {0};
    tokenize_args(buf, &ctx);

    if (strcmp(ctx.args[0], "read") == 0)   return shell_read_builtin(&ctx);
    if (strcmp(ctx.args[0], "dir") == 0)    return shell_dir_builtin(&ctx);
    if (strcmp(ctx.args[0], "reboot") == 0) sys_reboot();

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
    log_group_begin("ShellReadBuiltin");
    dir_entry* f = (void*)0;
    uint8_t* buf = (void*)0;

    log_info("ShellBuiltinType", "Read file\n");

    // 1. Convert input filename into a FAT12 compatible 8.3 format
    char name_8_3[12] = "           ";
    if (fat_drive_filename_to_8_3(arg_ctx->args[1], name_8_3) == -1) {
        return BUILTIN_RESULT(ERROR_GENERIC, (void*)0);
    }

    // 2. Search the file system for the 8.3 name
    if (!(f = fat_drive_find_file(name_8_3))) {
        return BUILTIN_RESULT(ERROR_FILE_NOT_FOUND, (void*)0);
    }

    // 3. Read the entry into the kernel arena (FUTURE: This should be the heap)
    buf = arena_alloc(kernel_arena, f->Size);
    if (!fat_drive_read_file(f, buf)) {
        log_info("Status", "Failed to read file\n");
        return BUILTIN_RESULT(ERROR_FILE_READ, (void*)0);
    }
    log_group_end("ShellReadBuiltin");

    return BUILTIN_RESULT(ERROR_NONE, (char*)buf);
}

struct builtin_result shell_dir_builtin(const struct argument_ctx* arg_ctx) {
    (void)arg_ctx;
    log_info("ShellBuiltin", "Dir\n");
    return BUILTIN_RESULT(ERROR_NONE, "Unimplemented");
}
