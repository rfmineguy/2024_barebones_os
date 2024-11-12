#include "timer.h"
#include <stdint.h>
#include "idt.h"
#include "io.h"
#include "vga.h"
#include "log.h"
#include "serial.h"

#define DATA_0 0x40         // Channel 0 data port (read/write)
#define DATA_2 0x42         // Channel 2 data port (read/write)
#define MODE_CMD 0x43       // Mode/Command register (write only, a read is ignored)

//https://wiki.osdev.org/Programmable_Interval_Timer
uint64_t ticks;
const uint32_t frequency = 100;

typedef struct {
    int trigger_interval;
    int (*listener)(int);
} timer_listener_data;

timer_listener_data listeners[10];
int listeners_count;

void timer_add_listener(int(* listener)(int), int interval) {
    if (listeners_count >= 10) {
        log_crit("KeyboardAddListener", "Couldn't add new timer listener");
        return;
    }
    listeners[listeners_count] = (timer_listener_data) {
        .listener = listener,
        .trigger_interval = interval
    };
    log_info("KeyboardAddListener", "Added new timer listener %d", listeners_count);
    listeners_count++;
}
void timer_init() {
    listeners_count = 0;
    idt_cli();
    ticks = 0;
    irq_install_handler(0, &timer_onirq0);

    // 1.1931816666Mhz
    // 119318.16666hz
    uint32_t divisor = 1193180/frequency;

    /***
     *    TODO: Look into why outb causes a crahs
     *    NOTE: Without this the PIT still ticks and the irq is called properly... idk
     */

    // 0x36 = 00110110
    // 00   = channel 0   (generates irq0, required for our above installed handler)
    // 11   = access mode (lo and hi byte)
    // 011  = square wave generator (denotes when we tick)
    // 0    = 16-bit binary mode
    io_outb(MODE_CMD, 0x36);
    io_outb(DATA_0, (uint8_t)(divisor & 0xff));
    io_outb(DATA_0, (uint8_t)((divisor >> 8) & 0xff));
}
void timer_onirq0(struct interrupt_registers_test* regs) {
    (void)(regs);   // not used in timer irq
    
    ticks++;
    for (int i = 0; i < listeners_count; i++) {
        timer_listener_data ld = listeners[i];
        if (ticks % ld.trigger_interval == 0 && ld.listener != NULL) {
            // ld.listener(ticks);
        }
    }
}
uint64_t timer_ticks() {
    return ticks;
}
