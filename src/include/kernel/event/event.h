#ifndef EVENT_H
#define EVENT_H
#include "drivers/keyboard.h"
#include <stdbool.h>

typedef enum event_type {
  MOUSE, KEYBOARD
} event_type;

typedef struct event {
  event_type type;
  union {
    struct {
      int8_t x, y;
      int8_t leftbtn, rightbtn, middlebtn;
      bool clicked, moved;
    } mouse;
    struct {
      char ch;
      char press;
      modifier_flags modifier_flags;
    } keyboard;
  };
} event;

void event_log(event);

#endif
