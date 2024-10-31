#include "keyboard.h"
#include "io.h"
#include "log.h"
#include "idt.h"
#include "../stdlib/printf.h"
#include "../stdlib/stdbool.h"
#include "../stdlib/stdint.h"

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

// array of listeners
int (*listeners[10])(char ch);
int listeners_count;

bool caps_on, capslock;
void keyboard_add_listener(int(* listener)(char)) {
    if (listeners_count >= 10) {
        log_crit("KeyboardAddListener", "Couldn't add new keyboard listener");
        return;
    }
    listeners[listeners_count] = listener;
    listeners_count++;
}
void keyboard_init() {
    idt_cli();
    caps_on = false;
    capslock = false;
    irq_install_handler(1, &keyboard_irq);

    for (int i = 0; i < 10; i++) listeners[i] = (void*)0;
    listeners_count = 0;
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
            for (int i = 0; i < listeners_count; i++) {
                if (press == 0) {
                    if (caps_on || capslock) listeners[i](uppercase[(int)scancode]);
                    else listeners[i](lowercase[(int)scancode]);
                }
            }
            break;
            // if (press == 0) {
            //     if (caps_on || capslock) vga_putch(uppercase[(int)scancode]);
            //     else vga_putch(lowercase[(int)scancode]);
            // }
    }
}
