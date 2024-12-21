#ifndef SHELL_V2_H
#define SHELL_V2_H
#include "mouse.h"
#include "ui.h"
/**
 * Brainstorm:
 * 		This shell's sole purpose for now is to display the mouse cursor
 */

int shell2_keyboard_listener(char, uint8_t);
int shell2_mouse_listener(mouse_device_packet_t);
int shell2_timer_listener(int);

int shell2_run(ui_box_t*);

#endif
