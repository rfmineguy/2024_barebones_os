#include "ui_v2.h"
#include "vga.h"
#include "extascii.h"
#include "../stdlib/stdarg.h"
#include "../stdlib/string.h"
#include "../stdlib/printf.h"
#include "../stdlib/ctype.h"
#include "../stdlib/stdbool.h"

// store state of entire screen here
//    the uibox's will index into their subregions
uint16_t ui_screen_buf[VGA_WIDTH * VGA_HEIGHT];

// Example:
//   list: []                         Add {0 , 0, 10, 10}
//   list: [{0,0,10,10}]              Add {11, 0, 10, 10}
//   list: [{0,0,10,10},{11,0,10,10}]
ui_region update_list[1000] = {0};
int update_list_len = 0;

ui_box2 ui2_new(int _x, int _y, int _w, int _h, const char* _title) {
    return (ui_box2) {
        .region = (ui_region) {
            .x=_x,.y=_y,.w=_w,.h=_h
        },
        .title = _title
    };
}
void ui2_putch_int(ui_box2* box, int x, int y, unsigned char c) {
    int bx = box->region.x + x;
    int by = box->region.y + y;
    switch (c) {
        case '\n':
        default:
            ui_screen_buf[by * VGA_WIDTH + bx] = vga_make_entry(c, vga_make_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
    }
    update_list[update_list_len++] = (ui_region){.x=bx, .y=by, .w=1, .h=1};
}
void ui2_putch(ui_box2* box, int x, int y, unsigned char c){
    ui2_putch_int(box, x + 1, y + 1, c);
}
void ui2_putstr_int(ui_box2* box, int x, int y, const char* str) {
    for (int i = 0; i < strlen(str); i++) {
        ui2_putch_int(box, x + i, y, str[i]);
    }
}
void ui2_putstr(ui_box2* box, int x, int y, const char* str){
    for (int i = 0; i < strlen(str); i++) {
        ui2_putch(box, x + i, y, str[i]);
    }
}
void ui2_printf(ui_box2* box, int x, int y, const char* fmt, ...){
    if (x < 0 || x >= box->region.w) return;
    if (y < 0 || y >= box->region.h) return;

    static char buf[500] = {0};
    va_list alist;
    va_start(alist, fmt);
    k_vsprintf(buf, fmt, alist);
    va_end(alist);

    char* bufptr = buf;
    int linespan = strlen(buf) % (box->region.w - 2); // how many lines will this string span
    for (int i = 0; i <= linespan; i++) {
        int linelen = strlen(buf) < box->region.w - 2 ? strlen(buf) : box->region.w -2;
        for (int i = 0; i < linelen; i++) {
            ui2_putch(box, x + i, y, bufptr[i]);
            bufptr += linelen;
        }
        y++;
        x = 0;
    }
}
void ui2_clear_r(ui_box2* box, ui_region r) {
    for (int i = r.x; i < r.x + r.w; i++) {
        for (int j = r.y; j < r.y + r.h; j++) {
            ui2_putch(box, i, j, ' ');
        }
    }
    update_list[update_list_len++] = r;
}
void ui2_clear(ui_box2* box){
    ui_region r = (ui_region) {
       .x = box->region.x + 1,
       .y = box->region.y + 1,
       .w = box->region.w - 2,
       .h = box->region.h - 2 
    };
    ui2_clear_r(box, r);
}
void ui2_clear_rv(ui_box2* box, int x, int y, int w, int h){
    ui_region r = (ui_region){.x=x, .y=y, .w=w, .h=h};
    ui2_clear_r(box, r);
}

void ui2_box(ui_box2* box) {
    ui_region r = box->region;
    ui2_putch_int(box, 0, 0, CTL);
    ui2_putch_int(box, r.w, 0, CTR);
    ui2_putch_int(box, 0, r.h, CBL);
    ui2_putch_int(box, r.w, r.h, CBR);
    for (int i = 1; i < r.w; i++) {
        ui2_putch_int(box, i, 0, HOR);
        ui2_putch_int(box, i, r.h, HOR);
    }
    for (int i = 1; i < r.h; i++) {
        ui2_putch_int(box, 0, i, VERT);
        ui2_putch_int(box, r.w, i, VERT);
    }
    ui2_putstr_int(box, 1, 0, box->title);
}

// go through update queue and show the changes to the real vga buffer
//  from the ui_screen_buf
void ui2_refresh() {
    if (update_list_len == 0) return;
    for (int i = 0; i < update_list_len; i++) {
        ui_region r = update_list[i];
        for (int j = r.y; j <= r.y + r.h; j++) {
            for (int i = r.x; i <= r.x + r.w; i++) {
                uint16_t new_entry = ui_screen_buf[j * VGA_WIDTH + i];
                vga_put_entry_at_v(new_entry, i, j);
            }
        }
    }
    update_list_len = 0; // we handled all the updates
}
