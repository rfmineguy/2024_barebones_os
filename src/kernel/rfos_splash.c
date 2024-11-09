#include "rfos_splash.h"
#include "ui.h"
#define R(x) #x

#define REP0(x)
#define REP1(x) (x)
#define REP2(x) REP1(x), (x)
#define REP3(x) REP2(x), (x)
#define REP4(x) REP3(x), (x)
#define REP5(x) REP4(x), (x)
#define REP6(x) REP5(x), (x)
#define REP7(x) REP6(x), (x)

//       R       F       O        S
//    |-----| |-----| |------| |-----|
// ╔═════════════════════════════════════╗
// ║  ██████  ███████  ██████  ███████   ║
// ║  ██   ██ ██      ██    ██ ██        ║
// ║  ██████  █████   ██    ██ ███████   ║
// ║  ██   ██ ██      ██    ██      ██   ║
// ║  ██   ██ ██       ██████  ███████   ║
// ╚═════════════════════════════════════╝
void rfos_splash(ui_box2* box) {
    ////                            R                     Space        F         Space                     O                   Space          S                                  S
    ////             |--------------------------------| |-------|  |--------|  |-------|  |--------------------------------| \-------|   |---------|           
    char line1[] = { REP6(BLCK),                        REP2(' '), REP7(BLCK), REP2(' '),             REP6(BLCK),            REP2(' '), REP7(BLCK), 0};
    char line2[] = { REP2(BLCK), REP3(' '), REP2(BLCK), REP1(' '), REP2(BLCK), REP6(' '), REP2(BLCK), REP4(' '), REP2(BLCK), REP1(' '), REP2(BLCK), 0};
    char line3[] = { REP6(BLCK),                        REP2(' '), REP5(BLCK), REP3(' '), REP2(BLCK), REP4(' '), REP2(BLCK), REP1(' '), REP7(BLCK), 0};
    char line4[] = { REP2(BLCK), REP3(' '), REP2(BLCK), REP1(' '), REP2(BLCK), REP6(' '), REP2(BLCK), REP4(' '), REP2(BLCK), REP1(' '), REP5(' '), REP2(BLCK), 0};
    char line5[] = { REP2(BLCK), REP3(' '), REP2(BLCK), REP1(' '), REP2(BLCK), REP7(' '),             REP6(BLCK),            REP2(' '), REP7(BLCK), 0};
    ui2_putstr(box, 2, 1, line1);
    ui2_putstr(box, 2, 2, line2);
    ui2_putstr(box, 2, 3, line3);
    ui2_putstr(box, 2, 4, line4);
    ui2_putstr(box, 2, 5, line5);
    ui2_putstr(box, 2, 7, "Welcome to RFOS!!!");
}
