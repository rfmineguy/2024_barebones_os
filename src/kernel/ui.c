#include "ui.h"
#include "../stdlib/stdarg.h"
#include "vga.h"
#include "../stdlib/printf.h"
#include "../stdlib/string.h"

void emit(int n, ...) {
    va_list alist;
    va_start(alist, n);
    for (int i = 0; i < n; i++) {
        struct emission e = va_arg(alist, struct emission);
        if (e.c == ' ') {
           vga_advance_colby(e.n);
           continue;
        }
        for (int x = 0; x < e.n; x++) {
            vga_putch(e.c);
        }
    }
    va_end(alist);
    vga_putch('\n');
}

void ui_box_draw(ui_box* box) {
    vga_put_ch_at(CTL, box->x, box->y);
    vga_put_ch_at(CTR, box->x + box->w, box->y);
    vga_put_ch_at(CBL, box->x, box->y + box->h);
    vga_put_ch_at(CBR, box->x + box->w, box->y + box->h);
    for (int i = box->x + 1; i < box->x + box->w; i++) {
        vga_put_ch_at(HOR, i, box->y);
        vga_put_ch_at(HOR, i, box->y + box->h);
    }
    for (int i = box->y + 1; i < box->y + box->h; i++) {
        vga_put_ch_at(VERT, box->x, i);
        vga_put_ch_at(VERT, box->x + box->w, i);
    }
}

void ui_box_putch(ui_box* box, int x, int y, char c) {
    if (x < 0 || y < 0) return;
    if (x > box->x + box->w - 2) return;
    if (y > box->y + box->h - 2) return; 

    int screen_x = box->x + x + 1;
    int screen_y = box->y + y + 1;
    vga_put_ch_at(c, screen_x, screen_y);
}

void ui_box_printf(ui_box* box, int x, int y, const char* fmt, ...) {
    static char buf[1000] = {0};
    if (x < 0 || y < 0) return;
    if (x > box->x + box->w - 2) return;
    if (y > box->y + box->h - 2) return; 
    box->cursorx = x;
    box->cursory = y;

    va_list alist;
    va_start(alist, fmt);
    k_vsprintf(buf, fmt, alist);
    va_end(alist);

    for (int i = 0; i < strlen(buf); i++) {
        if (box->cursorx > box->w - 2) {
            box->cursorx = 1;
            box->cursory++;
        }
        ui_box_putch(box, box->cursorx, box->cursory, buf[i]);
        box->cursorx++;
    }
    // if (box->cursory >= box->h - 2) {
    //     // scroll everything up (remove oldest line)
    //     for (int j = box->y + box->h - 2; j > box->y; j--) {
    //         for (int i = box->x + 1; i < box->x + box->w - 2; i++) {
    //             vga_put_ch_at(vga_getch(i, j + 1), i, j);
    //         }
    //     }
    // }
}

void ui_box_clearline(ui_box* box, int line) {
    for (int i = box->x; i < box->x + box->w; i++) {
        vga_put_ch_at(' ', i, box->y + 1 + line);
    }
}

void ui_box_clear(ui_box* box) {
    size_t oldx, oldy;
    vga_save_cursor(&oldx, &oldy);
    vga_put_cursor_at(1 + box->x, box->y);
    for (int i = box->x; i < box->x + box->w; i++) {
    }
    vga_put_cursor_at(oldx, oldy);
}

void ui_box_clearregion(ui_box* box, int x, int y, int w, int h) {
    int boxx = box->x + x;
    int boxy = box->y + y;
    for (int i = boxx + 1; i < boxx + w; i++) {
        for (int j = boxy + 1; j <= boxy + h; j++) {
            vga_put_ch_at(' ', i, j);
        }
    }
}
