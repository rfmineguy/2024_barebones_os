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
bool return_pressed = false, backspace_pressed = false, other_pressed = false;
char shell_buffer[1024] = {0};
int shell_buffer_i = 0;
arena* kernel_arena = (void*)0;

int shell_keyboard_listener(char ch, uint8_t mods) {
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
       case 9:    log_info("Key", "Tab pressed");
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
                if (r.is_clear_command) {
                    ui_scroll_vertical_n(box, current_line);
                    current_line = 0;
                    break;
                }
                for (int i = 0; i < r.string_result_count; i++) {
                    output_linecount += ui_putstr(box, 2, current_line + output_linecount, r.string_results[i]);
                }
                break;
            }
            default:
                output_linecount = ui_putstr(box, 2, current_line, map_error_code(r.code));
                break;
            }

            // scroll if needed
            if (current_line + output_linecount >= 10) {
                ui_scroll_vertical_n(box, output_linecount + 1);
                current_line --;
            }
            else {
                current_line += output_linecount;
            }
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

struct builtin_result shell_process(char* buf) {
    struct argument_ctx ctx = {0};
    tokenize_args(buf, &ctx);

    for (int i = 0; i < ctx.arg_counter; i++) {
        log_info("Arg", "%d = %s", i, ctx.args[i]);
    }

    if (strcmp(ctx.args[0], "read") == 0)   return shell_read_builtin(&ctx);
    if (strcmp(ctx.args[0], "list") == 0)   return shell_list_builtin(&ctx);
    if (strcmp(ctx.args[0], "newf") == 0)   return shell_newf_builtin(&ctx);
    if (strcmp(ctx.args[0], "delf") == 0)   return shell_delf_builtin(&ctx);
    if (strcmp(ctx.args[0], "appf") == 0)   return shell_appf_builtin(&ctx);
    if (strcmp(ctx.args[0], "clear") == 0)  return (struct builtin_result) {.is_clear_command = true};
    if (strcmp(ctx.args[0], "reboot") == 0) sys_reboot();

    return BUILTIN_RESULT_SUC_NO_MSG(ERROR_INVALID_CMD); // invalid command supplied
}

struct builtin_result shell_read_builtin(const struct argument_ctx* arg_ctx) {
    log_group_begin("ShellReadBuiltin");
    dir_entry* f = (void*)0;
    uint8_t* buf = (void*)0;

    log_info("ShellBuiltinType", "Read file");

    // 1. Convert input filename into a FAT12 compatible 8.3 format
    char name_8_3[12] = "           ";
    if (fat_drive_filename_to_8_3(arg_ctx->args[1], name_8_3) == -1) {
        log_group_end("ShellReadBuiltin");
        return BUILTIN_RESULT_SUC_NO_MSG(ERROR_GENERIC);
    }

    // 2. Search the file system for the 8.3 name
    if (!(f = fat_drive_find_file(name_8_3))) {
        log_group_end("ShellReadBuiltin");
        return BUILTIN_RESULT_SUC_NO_MSG(ERROR_FILE_NOT_FOUND);
    }

    // 3. Read the entry into the kernel arena (FUTURE: This should be the heap)
    buf = arena_alloc(kernel_arena, f->Size);
    if (!fat_drive_read_file(f, buf)) {
        log_info("Status", "Failed to read file");
        log_group_end("ShellReadBuiltin");
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

        r.string_results[i - 1] = arena_alloc(kernel_arena, 12);
        fat_drive_8_3_to_filename((const char*)root_dir[i].Name, r.string_results[i - 1]);

        log_info("Entry", "%s", r.string_results[i - 1]);
        r.string_result_count++;
    }
    log_group_end("ShellListBuiltin");
    return r;
}

struct builtin_result shell_newf_builtin(const struct argument_ctx* arg_ctx) {
    log_group_begin("Shell 'newf'");
    char name_8_3[12] = {0};
    fat_drive_filename_to_8_3(arg_ctx->args[1], name_8_3);
    log_info("Norm", "\"%s\"", arg_ctx->args[1]);
    log_info("8.3", "\"%s\"", name_8_3);
    log_group_end("Shell 'newf'");
    return BUILTIN_RESULT_ERR(ERROR_UNIMPLEMENTED, "WIP");
}

struct builtin_result shell_appf_builtin(const struct argument_ctx* arg_ctx) {
    log_group_begin("Shell 'appf'");
    char name_8_3[12] = {0};
    dir_entry* f = (void*)0;
    fat_drive_filename_to_8_3(arg_ctx->args[1], name_8_3);
    if (!(f = fat_drive_find_file(name_8_3))) {
        log_group_end("Shell 'appf'");
        return BUILTIN_RESULT_ERR(ERROR_FILE_NOT_FOUND, "");
    }
    log_info("Append", "%s", name_8_3);

    uint8_t fbuf[512]; // enough storage for a full sector (files max size is 512)
    if (!fat_drive_read_file(f, fbuf)) {
        log_info("Status", "Failed to read file");
        log_group_end("ShellReadBuiltin");
        return BUILTIN_RESULT_ERR(ERROR_FILE_READ, 0);
    }

    // Error if the appended string would overflow the file
    if (f->Size + strlen(arg_ctx->args[2]) > 512) {
        log_crit("Status", "File too big, can't write");
        return BUILTIN_RESULT_ERR(ERROR_FILE_TOO_BIG, 0);
    }

    // Appened argument argument to file buffer
    for (int i = 0; i < strlen(arg_ctx->args[2]); i++)
        fbuf[f->Size + i] = arg_ctx->args[2][i];

    // Write fbuf to the lba that f resides
    uint16_t current_cluster = f->FirstClusterLow;
    uint32_t root_end = fat_drive_internal_get_root_dir_end();
    boot_sector bs = fat_drive_internal_get_boot_sector();
    uint32_t lba = (root_end + (current_cluster - 2)) * bs.SectorsPerCluster;
    fat_drive_write_sectors(lba, 1, fbuf);
    log_info("Appf", "Write to lba: %d", lba);

    // Update the root directory to reflect the file's new size
    dir_entry* root = fat_drive_internal_get_root_dir_mut();
    log_info("Appf", "Rootdir size: %d", root->Size);

    for (int i = 0; i < fat_drive_internal_get_boot_sector().DirEntryCount; i++) {
        if (strncmp((const char*)root[i].Name, name_8_3, 11) == 0) {
            log_info("Appf", "Found root dir entry %11s at entry %d", name_8_3, i);
            root[i].Size = f->Size + strlen(arg_ctx->args[2]);
            break;
        }
    }

    // Write to the root directory
    boot_sector boot = fat_drive_internal_get_boot_sector();
    lba = boot.ReservedSectors + boot.SectorsPerFat * boot.FatCount;
    uint32_t size = sizeof(dir_entry) * boot.DirEntryCount;
    uint32_t sectors = (size / boot.BytesPerSector);
    if (size % boot.BytesPerSector > 0) sectors++;
    fat_drive_write_sectors(lba, sectors, root);

    log_group_end("Shell 'appf'");
    return BUILTIN_RESULT_SUC(ERROR_NONE, "Wrote the file");
}

struct builtin_result shell_delf_builtin(const struct argument_ctx* arg_ctx) {
    return BUILTIN_RESULT_ERR(ERROR_UNIMPLEMENTED, "Not implemented");
}
