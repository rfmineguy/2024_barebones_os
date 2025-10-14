#include "keyboard.h"
#include "io.h"
#include "log.h"
#include "idt.h"
#include "../stdlib/printf.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/stdint.h"
#include "event_system.h"

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
    unsigned char scancode = io_inb(0x60) & 0x7F; // scancode of key
    unsigned char press    = io_inb(0x60) & 0x80;

    modifier_flags modifier_flags = 0x0;
    switch (scancode) {
        case 1:
        case 0x1D: modifier_flags |= L_CONTROL;
                   break;
        // case 0x2A: modifier_flags |= L_SHIFT;
        //            break;
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
        default: {
            bool caps = caps_on || capslock;
            char ch = caps ? uppercase[(int)scancode] : lowercase[(int)scancode];
            log_info("Key IRQ", "Press: %d, ch: %c\n", press, ch);
            event_system_post(((event) {
                .type = KEYBOARD, // keyboard
                .keyboard = {
                  .ch = ch,
                  .modifier_flags = modifier_flags,
                  .press = press,
                }})
            );
            break;
          };
    }
}
