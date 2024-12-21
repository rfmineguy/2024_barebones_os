#ifndef UIV2_H
#define UIV2_H
#include "../stdlib/stdint.h"

/* UIV2
 *   This version of UI will write into memory rather than directly
 *   to the vga buffer.
 *   Hopefully this will mitigate somewhat the problem of flickering.
 *     - There should be a concept of updating only regions that require updating
 *        to mitigate unncessarily processing more than needed via a "change_list?"
*/
typedef struct ui_region {
    int parent_boxid;
    int x, y;
    int w, h;
} ui_region;

typedef struct ui_box {
    uint8_t border_color, body_color;

    int boxid;
    const char* title;
    ui_region region;     // region of this uibox
} ui_box_t;

ui_box_t ui_new             (int x, int y, int w, int h, const char*);
void     ui_set_border_color(ui_box_t*, uint8_t fg, uint8_t bg);
void     ui_set_body_color  (ui_box_t*, uint8_t fg, uint8_t bg);

char 	   ui_getch_r(ui_box_t*, int x, int y);
char 		 ui_getch(ui_box_t*, int x, int y);

void     ui_putch_r(ui_box_t*, int x, int y, unsigned char c);
void     ui_putch  (ui_box_t*, int x, int y, unsigned char c);
int      ui_putstr (ui_box_t*, int x, int y, const char*);
void     ui_printf (ui_box_t*, int x, int y, const char*, ...);

void     ui_fill     (ui_box_t*, char c);
void     ui_clear_r  (ui_box_t*, ui_region);
void     ui_clear    (ui_box_t*);
void     ui_clear_rv (ui_box_t*, int x, int y, int w, int h);

int      ui_scroll_vertical_n(ui_box_t*, int);
int      ui_scroll_vertical(ui_box_t*);

void     ui_box      (ui_box_t*);
void     ui_refresh  ();
void     ui_refresh_b(ui_box_t*);

#endif
