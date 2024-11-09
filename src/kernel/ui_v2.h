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
    int x, y;
    int w, h;
} ui_region;

typedef struct ui_box2 {
    const char* title;
    ui_region region;     // region of this uibox
} ui_box2;

ui_box2 ui2_new    (int x, int y, int w, int h, const char*);

void    ui2_putch  (ui_box2*, int x, int y, unsigned char c);
void    ui2_putstr (ui_box2*, int x, int y, const char*);
void    ui2_printf (ui_box2*, int x, int y, const char*, ...);

void    ui2_clear_r  (ui_box2*, ui_region);
void    ui2_clear    (ui_box2*);
void    ui2_clear_rv (ui_box2*, int x, int y, int w, int h);

void    ui2_box      (ui_box2*);
void    ui2_refresh  ();

#endif
