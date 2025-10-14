#include "shell_2.h"
#include "log.h"
#include "idt.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/printf.h"
#include "ctype.h"
#include "extascii.h"
#include "event_system.h"
#include "string.h"

int frames;
int8_t last_cursor_x, last_cursor_y;
int8_t cursor_x, cursor_y;
bool cursor_moved, middle_clicked, left_clicked, right_clicked;

void shell2_show_cursor_data(ui_box_t* box) {
  static char buf[512];

  ui_putstr(box, 0, 0, buf);
  k_sprintf(buf, "%d, %d        ", cursor_x, cursor_y);
  ui_putstr(box, 0, 1, buf);
}

void shell2_show_cursor_pos(ui_box_t* box, char* last_frame) {
  ui_putch_r(box, last_cursor_x, last_cursor_y, *last_frame);
  *last_frame = ui_getch_r(box, cursor_x, cursor_y);
  ui_putch_r(box, cursor_x, cursor_y, BLCK);
}

int shell2_run(ui_box_t* box) {
  char buf[500] = {0};
  int buf_i = 0;
  cursor_x = 39;
  cursor_y = 10;
  frames = 0;
  ui_box(box);
  char last_frame = ' ';

  while (true) {
    event e;
    if (event_system_get(&e)) {
      event_log(e);
      switch (e.type) {
        case KEYBOARD:
          {
            log_info("Shell2", "Keyboard");
            if (e.keyboard.press == 0) {
              log_info("Shell2", "ch=%d, %c", e.keyboard.ch, e.keyboard.ch);
              if (e.keyboard.ch == 8) { // backspace
                if (buf_i > 0) buf[--buf_i] = 0;
              }
              else {
                buf[buf_i++] = e.keyboard.ch;
                buf[buf_i] = 0;
              }
              ui_clear_rv(box, 0, 10, buf_i + 2, 2);
              ui_putstr(box, 0, 10, buf);
              ui_putstr(box, strlen(buf), 11, "_");
            }
          }
          break;
        case MOUSE:
          {
            if (e.mouse.moved == 1) {
              last_cursor_x = cursor_x;
              last_cursor_y = cursor_y;
              cursor_x = e.mouse.x;
              cursor_y = 24 - e.mouse.y;
              shell2_show_cursor_pos(box, &last_frame);
              ui_refresh_b(box);
            }

            if (e.mouse.clicked == 1) {
              ui_putch(box, e.mouse.x, 24 - e.mouse.y, BLCK);
            }
          }
          break;
      }
    }
    shell2_show_cursor_data(box);

    frames ++;
    ui_refresh();
  }
  return 0;
}
