#include "tips.h"

void tips_populate(ui_box_t* box) {
    ui_putstr(box, 1, 0, "read <file>");
    ui_putstr(box, 1, 1, "newf <file>");
    ui_putstr(box, 1, 2, "appf <file> <text>");
    ui_putstr(box, 1, 4, "list");
    ui_putstr(box, 1, 5, "reboot");
}
