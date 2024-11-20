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
bool up_pressed = false,down_pressed = false;
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
    cursor_on = !cursor_on;
    return 0;
}

int shell_run(arena* _kernel_arena, ui_box_t* box) {
    kernel_arena = _kernel_arena;

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
            case ERROR_NONE: {
                for (int i = 0; i < r.string_result_count; i++) {
                    // log_info("Shell", "Line #%d : \"%s\"", i, r.string_results[i]);
                    // fat_drive_8_3_to_filename(r.string_results[i], filename);
                    output_linecount += ui_putstr(box, 2, current_line + output_linecount, r.string_results[i]);
                }
                break;
            }
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

// arguments can be passed as such
//    dada fa da "daf dafkhda" ad
//
//    this will parsed as
//    ["dada", "fa", "da", "daf dafkhda", "ad"]
static void tokenize_args(char* str, struct argument_ctx* ctx) {
    ctx->arg_counter = 0;

    char* start = str;
    while (*str != 0) {
        if (*str == '"') {
            str++;
            start = str;
            while (*str != 0 && *str != '"') str++;
            if (*str == 0) { /* unterminated quote */ }
            *str = 0;
            ctx->args[ctx->arg_counter] = start + 1;
        }
        else if (*str == ' ') {
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

    for (int i = 0; i < ctx.arg_counter; i++) {
        log_info("Arg", "%d = %s", i, ctx.args[i]);
    }

    if (strcmp(ctx.args[0], "read") == 0)   return shell_read_builtin(&ctx);
    if (strcmp(ctx.args[0], "list") == 0)   return shell_list_builtin(&ctx);
    if (strcmp(ctx.args[0], "reboot") == 0) sys_reboot();

    return BUILTIN_RESULT_SUC_NO_MSG(ERROR_INVALID_CMD); // invalid command supplied
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

    log_info("ShellBuiltinType", "Read file");

    // 1. Convert input filename into a FAT12 compatible 8.3 format
    char name_8_3[12] = "           ";
    if (fat_drive_filename_to_8_3(arg_ctx->args[1], name_8_3) == -1) {
        return BUILTIN_RESULT_SUC_NO_MSG(ERROR_GENERIC);
    }

    // 2. Search the file system for the 8.3 name
    if (!(f = fat_drive_find_file(name_8_3))) {
        return BUILTIN_RESULT_SUC_NO_MSG(ERROR_FILE_NOT_FOUND);
    }

    // 3. Read the entry into the kernel arena (FUTURE: This should be the heap)
    buf = arena_alloc(kernel_arena, f->Size);
    if (!fat_drive_read_file(f, buf)) {
        log_info("Status", "Failed to read file");
        return BUILTIN_RESULT_SUC(ERROR_FILE_READ, 0);
    }
    log_group_end("ShellReadBuiltin");

    return BUILTIN_RESULT_SUC(ERROR_NONE, (char*)buf);
}

struct builtin_result shell_list_builtin(const struct argument_ctx* arg_ctx) {
    log_group_begin("ShellListBuiltin");
    (void)arg_ctx;
    const dir_entry* root_dir = fat_drive_internal_get_root_dir();
    struct builtin_result r = {0};
    char filename[11];
    for (uint32_t i = 1; i < fat_drive_internal_get_boot_sector().DirEntryCount; i++) {
        if (root_dir[i].Name[0] == 0x00) break;

        // put name into result area
        r.string_results[i - 1] = arena_alloc(kernel_arena, 12);
        fat_drive_8_3_to_filename((const char*)root_dir[i].Name, r.string_results[i - 1]);

        log_info("Entry", "%s", r.string_results[i - 1]);
        r.string_result_count++;
    }
    log_group_end("ShellListBuiltin");
    return r;
}
