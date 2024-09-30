#include "timer.h"
#include <stdint.h>
// #include "memory.h"
#include "printf.h"
#include "log.h"
#include "idt.h"
#include "io.h"

#define DATA_0 0x40         // Channel 0 data port (read/write)
#define DATA_2 0x42         // Channel 2 data port (read/write)
#define MODE_CMD 0x43       // Mode/Command register (write only, a read is ignored)

//https://wiki.osdev.org/Programmable_Interval_Timer
uint64_t ticks;
const uint32_t frequency = 100;

void timer_init() {
    // log_info("TimerInit", "Begin\n");
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
    // io_outb(MODE_CMD, 0x36); 
    // io_wait();

    // set the frequency
    // io_outb(DATA_0, (uint8_t)(divisor & 0xff));
    // io_wait();
    // io_outb(DATA_0, (uint8_t)((divisor >> 8) & 0xff));
    // io_wait();

    // log_info("TimerInit", "Finished\n");
}
void timer_onirq0(struct interrupt_registers_test* regs) {
    (void)(regs);   // not used in timer irq
    
    ticks++;
    if (ticks % 10 == 0) {
        //log_info("TimerIRQ0", "Ticks: %d\n", ticks);
    }
}
