#include "keyboard.h"
#include "io.h"
#include "log.h"
#include "idt.h"
#include "../stdlib/printf.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/stdint.h"

#define UNKNOWN  0xFFFFFFFF
#define ESC  0xFFFFFFFF - 1
#define CTRL  0xFFFFFFFF - 2
#define LSHFT  0xFFFFFFFF - 3
#define RSHFT  0xFFFFFFFF - 4
#define ALT  0xFFFFFFFF - 5
#define F1  0xFFFFFFFF - 6
#define F2  0xFFFFFFFF - 7
#define F3  0xFFFFFFFF - 8
#define F4  0xFFFFFFFF - 9
#define F5  0xFFFFFFFF - 10
#define F6  0xFFFFFFFF - 11
#define F7  0xFFFFFFFF - 12
#define F8  0xFFFFFFFF - 13
#define F9  0xFFFFFFFF - 14
#define F10  0xFFFFFFFF - 15
#define F11  0xFFFFFFFF - 16
#define F12  0xFFFFFFFF - 17
#define SCRLCK  0xFFFFFFFF - 18
#define HOME  0xFFFFFFFF - 19
#define UP  0xFFFFFFFF - 20
#define LEFT  0xFFFFFFFF - 21
#define RIGHT  0xFFFFFFFF - 22
#define DOWN  0xFFFFFFFF - 23
#define PGUP  0xFFFFFFFF - 24
#define PGDOWN  0xFFFFFFFF - 25
#define END  0xFFFFFFFF - 26
#define INS  0xFFFFFFFF - 27
#define DEL  0xFFFFFFFF - 28
#define CAPS  0xFFFFFFFF - 29
#define NONE  0xFFFFFFFF - 30
#define ALTGR  0xFFFFFFFF - 31
#define NUMLCK  0xFFFFFFFF - 32

const uint32_t lowercase[128] = {
    UNKNOWN,ESC,'1','2','3','4','5','6','7','8',
    '9','0','-','=','\b','\t','q','w','e','r',
    't','y','u','i','o','p','[',']','\n',CTRL,
    'a','s','d','f','g','h','j','k','l',';',
    '\'','`',LSHFT,'\\','z','x','c','v','b','n','m',',',
    '.','/',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',LEFT,UNKNOWN,RIGHT,
    '+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};
const uint32_t uppercase[128] = {
    UNKNOWN,ESC,'!','@','#','$','%','^','&','*','(',')','_','+','\b','\t','Q','W','E','R',
    'T','Y','U','I','O','P','{','}','\n',CTRL,'A','S','D','F','G','H','J','K','L',':','"','~',LSHFT,'|','Z','X','C',
    'V','B','N','M','<','>','?',RSHFT,'*',ALT,' ',CAPS,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,NUMLCK,SCRLCK,HOME,UP,PGUP,'-',
    LEFT,UNKNOWN,RIGHT,'+',END,DOWN,PGDOWN,INS,DEL,UNKNOWN,UNKNOWN,UNKNOWN,F11,F12,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,
    UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN,UNKNOWN
};


bool caps_on, capslock;
void keyboard_init() {
    idt_cli();
    caps_on = false;
    capslock = false;
    irq_install_handler(1, &keyboard_irq);
}
void keyboard_irq(struct interrupt_registers_test* regs) {
    (void)(regs);
    char scancode = io_inb(0x60) & 0x7F; // scancode of key
    char press    = io_inb(0x60) & 0x80;
    // log_info("Key IRQ", "Press: %d, Scan code: %d\n", press, scancode);

    switch (scancode) {
        case 1:
        case 29:
        case 56:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 87:
        case 88:
            break;
        case 42: // shift key
            caps_on = press == 0;
            break;
        case 58: // caps lock key
            if (press == 0) capslock = !capslock;
            // if (!capslock && press == 0) capslock = true;
            // else if (capslock && press == 0) capslock = false;
            break;
        default:
            if (press == 0) {
                if (caps_on || capslock) vga_putch(uppercase[(int)scancode]);
                else vga_putch(lowercase[(int)scancode]);
            }
    }
}
