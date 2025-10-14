#include "event.h"
#include "log.h"

void event_log(event e) {
  switch (e.type) {
    case MOUSE:
      {
        log_info("Event", "%d, Mouse{x=%d, y=%d, left=%d, middle=%d, right=%d, clicked=%d, moved=%d}",
            e.type,
            e.mouse.x, e.mouse.y, e.mouse.leftbtn, e.mouse.middlebtn, e.mouse.rightbtn,
            e.mouse.clicked, e.mouse.moved);
        break;
      }
    case KEYBOARD:
      {
        log_info("Event", "%d, Keyboard{ch=%c, press=%d, mod_flags=%b}",
            e.type,
            e.keyboard.ch, e.keyboard.press, e.keyboard.modifier_flags);
        break;
      }
  }
}
