#include "shell_2.h"
#include "log.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/printf.h"

int last_cursor_x, last_cursor_y;
int8_t cursor_x, cursor_y;
bool cursor_moved;

mouse_device_packet_t g_mouse_data;

int shell2_keyboard_listener(char ch, uint8_t mods) {}

int shell2_mouse_listener(mouse_device_packet_t mouse_data) {
	g_mouse_data = mouse_data;
	last_cursor_x = cursor_x;
	last_cursor_y = cursor_y;

	cursor_x += mouse_data.x_difference / 4;
	cursor_y += mouse_data.y_difference / 4;
	if (cursor_x <= 0) cursor_x = 0;
	if (cursor_x >= 79) cursor_x = 79;
	if (cursor_y <= 0) cursor_y = 0;
	if (cursor_y >= 24) cursor_y = 24;

	cursor_moved = true;

	return 0;
}

int shell2_timer_listener(int ticks) {}

void shell2_show_cursor_data(ui_box_t* box) {
	static char buf[512];

	k_sprintf(buf, "%d, %d        ", 
			g_mouse_data.x_delta_signed / 8,
			g_mouse_data.y_delta_signed / 8);
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
