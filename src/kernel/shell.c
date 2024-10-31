#include "shell.h"
#include "log.h"
#include "../stdlib/stdbool.h"
#include "vga.h"
#include "keyboard.h"

bool return_pressed = false;
char shell_buffer[1024] = {0};
int shell_buffer_i = 0;

int shell_keyboard_listener(char ch) {
   return_pressed = false;
   switch (ch) {
       case '\n': log_info("ShellKbd", "Return key\n");
                  return_pressed = true;
                  break;
       case 8:    log_info("ShellKbd", "Delete key\n");
                  if (shell_buffer_i <= 0) {
                      return ch;
                  }
                  shell_buffer_i--;
                  shell_buffer[shell_buffer_i] = 0;
                  break;
       default:   log_info("ShellKbd", "Got key %d\n", ch);
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
            log_info("Shell", "Processing '%s'\n", shell_buffer);

            // clear buffer
            shell_buffer_i = 0;
            shell_buffer[0] = 0;
            for (int i = 0; i < 80; i++) shell_buffer[i] = 0;
        }
    }
    return exit_code;
}
