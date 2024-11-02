#include "vga.h"
#include "log.h"
#include "../stdlib/string.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
// #if !defined(__i386__)
// #error "This tutorial needs to be compiled with a ix86-elf compiler"
// #endif

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char ch, uint8_t color) {
    return (uint16_t) ch | (uint16_t) color << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
size_t row, col;
uint8_t term_color;
uint16_t* buffer;

int cursor_blink_status;

void vga_init(){
    row = 0;
    col = 0;
    cursor_blink_status = 1;
    term_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    buffer = (uint16_t*) 0xB8000;
    for (size_t i = 0; i < VGA_WIDTH; i++) {
        for (size_t j = 0; j < VGA_WIDTH; j++) {
            buffer[j * VGA_WIDTH + i] = vga_entry(' ', term_color);
        }
    }
    log_info("VGA Init   ", "Initialized the vga buffer\n");
}

void vga_set_color(uint8_t color) {
    term_color = color;
}

void vga_save_cursor(size_t* x, size_t* y) {
    if (x != NULL) *x = col;
    if (y != NULL) *y = row;
}

int vga_put_cursor_at(size_t x, size_t y) {
    if (x >= VGA_WIDTH) x = 0;
    if (y >= VGA_HEIGHT) y = 0;
    row = y;
    col = x;
    return 0;
}

void vga_put_entry_at(char ch, uint8_t color, size_t x, size_t y) {
    buffer[y * VGA_WIDTH + x] = vga_entry(ch, color);
}

void vga_put_ch_at(char ch, size_t x, size_t y) {
    buffer[y * VGA_WIDTH + x] = vga_entry(ch, term_color);
}

void vga_putch(char ch) {
    switch (ch) {
        case '\n': row++;
                   vga_put_entry_at(' ', term_color, col, row-1); 
                   col=0;
                   break;
        case '\r': col=0;
                   break;
        case '\b': col--;
                   vga_put_entry_at(' ', term_color, col, row);
                   break;
        case '\t': col+=4; 
                   break;
        default:
           vga_put_entry_at(ch, term_color, col, row);
           col++;
           break;
    }
    if (row % VGA_HEIGHT == 0) {
        row = 0;
    }
    if (col % VGA_WIDTH == 0) {
        col = 0;
    }
}

void vga_write(const char* str, size_t len) {
    for (size_t i = 0; i < len; i++)
        vga_putch(str[i]);
}

void vga_writestring(const char* str){
    vga_write(str, strlen(str));
}

void vga_clearline() {
    size_t x, y;
    vga_save_cursor(&x, &y);
    vga_put_cursor_at(row, 0);
    for (int i = 0; i < VGA_WIDTH; i++) {
        vga_putch(' ');
    }
    vga_put_cursor_at(x, y);
}

void vga_scroll(int scroll) {
    (void)(scroll);
}

void vga_toggle_cursor_blink() {
    cursor_blink_status = cursor_blink_status == 0 ? 1 : 0;
    if (cursor_blink_status == 0) vga_put_entry_at((char)219, term_color, col, row);
    if (cursor_blink_status == 1) vga_put_entry_at(' ', term_color, col, row);
}
