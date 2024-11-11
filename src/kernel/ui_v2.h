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

typedef struct ui_box2 {
    uint8_t border_color, body_color;

    int boxid;
    const char* title;
    ui_region region;     // region of this uibox
} ui_box2;

ui_box2 ui2_new             (int x, int y, int w, int h, const char*);
void    ui2_set_border_color(ui_box2*, uint8_t fg, uint8_t bg);
void    ui2_set_body_color  (ui_box2*, uint8_t fg, uint8_t bg);

void    ui2_putch  (ui_box2*, int x, int y, unsigned char c);
int     ui2_putstr (ui_box2*, int x, int y, const char*);
void    ui2_printf (ui_box2*, int x, int y, const char*, ...);

void    ui2_fill     (ui_box2*, char c);
void    ui2_clear_r  (ui_box2*, ui_region);
void    ui2_clear    (ui_box2*);
void    ui2_clear_rv (ui_box2*, int x, int y, int w, int h);

int     ui2_scroll_vertical_n(ui_box2*, int);
int     ui2_scroll_vertical(ui_box2*);

void    ui2_box      (ui_box2*);
void    ui2_refresh  ();
void    ui2_refresh_b(ui_box2*);

#endif
