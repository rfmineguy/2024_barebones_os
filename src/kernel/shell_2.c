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
	ui_putch_r(box, cursor_x, cursor_y, '#');
	ui_refresh();
	cursor_moved = false;
}

int shell2_run(ui_box_t* box) {
	cursor_x = 39;
	cursor_y = 10;
	ui_box(box);

	char last_frame = ' ';
	while (1) {
		shell2_show_cursor_data(box);
		if (cursor_moved) {
			shell2_show_cursor_pos(box, &last_frame);
		}
	}
}
