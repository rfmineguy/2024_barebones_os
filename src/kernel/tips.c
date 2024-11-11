#include "tips.h"

void tips_populate(ui_box2* box) {
    ui2_putstr(box, 1, 0, "read <file>");
    ui2_putstr(box, 1, 1, "reboot");
}
