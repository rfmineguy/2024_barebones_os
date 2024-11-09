#ifndef VGA_H
#define VGA_H
#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

uint16_t vga_make_entry(unsigned char, uint8_t);
uint16_t vga_make_entry_color(enum vga_color, enum vga_color);

void vga_init();

uint16_t vga_get_entry(int, int);
void vga_set_color(uint8_t);

void vga_advance_colby(int);
void vga_advance_rowby(int);
void vga_save_cursor(size_t*, size_t*);
int  vga_put_cursor_at(size_t, size_t);
void vga_put_entry_at(char, uint8_t, size_t, size_t);
void vga_put_entry_at_v(uint16_t, size_t, size_t);
void vga_put_ch_at(char, size_t, size_t);
void vga_putch(char);
void vga_write(const char*, size_t);
void vga_writestring(const char*);
void vga_clearline();

void vga_scroll(int);

// 1 = on, 0 = off
void vga_toggle_cursor_blink();

#endif
