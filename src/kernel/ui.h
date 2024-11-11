#ifndef UI_H
#define UI_H
#include "extascii.h"

typedef struct emission {
    int n;
    char c;
} emission;
#define E(_n, _c) (struct emission) {.n = _n, .c = _c}

typedef struct ui_box {
    int x, y;
    int w, h;
    int cursorx, cursory;
} ui_box;

#define ui_box_new(x_, y_, w_, h_)\
    (ui_box) {.x=x_, .y=y_, .w=w_, .h=h_, .cursorx=0, .cursory=0}

void   ui_box_draw(ui_box*);
void   ui_box_printf(ui_box*, int, int, const char*, ...);
void   ui_box_clear(ui_box*);
void   ui_box_clearline(ui_box*, int);
void   ui_box_putch(ui_box* box, int x, int y, char c);
void   ui_box_clearregion(ui_box*, int x, int y, int w, int h);
void   ui_box_scroll_vertical(ui_box*);

void   emit(int n, ...);

#endif
