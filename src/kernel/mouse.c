#include "mouse.h"

/* vim: tabstop=4 shiftwidth=4 noexpandtab
 * Mouse driver
 */
#include "log.h"
#include "io.h"
#include "mouse.h"
#include "idt.h"

uint8_t mouse_cycle = 0;
int8_t  mouse_byte[3];

#define MOUSE_IRQ 12

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08

int (*mouse_listeners[10])(mouse_device_packet_t);
int mouse_listeners_count;

void mouse_wait(uint8_t a_type) {
	uint32_t timeout = 100000;
	if (!a_type) {
		while (--timeout) {
			if ((io_inb(MOUSE_STATUS) & MOUSE_BBIT) == 1) {
				return;
			}
		}
		log_info("", "mouse timeout");
		return;
	} else {
		while (--timeout) {
			if (!((io_inb(MOUSE_STATUS) & MOUSE_ABIT))) {
				return;
			}
		}
		log_info("", "mouse timeout");
		return;
	}
}

void mouse_write(uint8_t write) {
	mouse_wait(1);
	io_outb(MOUSE_STATUS, MOUSE_WRITE);
	mouse_wait(1);
	io_outb(MOUSE_PORT, write);
}

uint8_t mouse_read() {
	mouse_wait(0);
	char t = io_inb(MOUSE_PORT);
	return t;
}

void mouse_add_listener(int(*listener)(mouse_device_packet_t)) {
    if (mouse_listeners_count >= 10) {
        log_crit("KeyboardAddListener", "Couldn't add new keyboard listener");
        return;
    }
    mouse_listeners[mouse_listeners_count] = listener;
    mouse_listeners_count++;
}

void mouse_handler(struct interrupt_registers_test *r) {
	mouse_byte[mouse_cycle++] = mouse_read();

	if (mouse_cycle == 3) {
		mouse_cycle = 0;
		int8_t info = mouse_byte[0];
		int8_t mouse_dx = mouse_byte[1];
		int8_t mouse_dy = mouse_byte[2];

		if ((info & 0x8) != 0x8) {
            log_warn("Mouse", "Desynchronized packet. Attempting resync.");
            
            // Shift buffer left by one byte and continue
            mouse_byte[0] = mouse_byte[1];
            mouse_byte[1] = mouse_byte[2];
			mouse_cycle = 2;
            return;
			// log_info("Mouse", "Something wrong");
			// mouse_cycle = 0;
			// return;
		}

		log_info("Mouse", "Raw [%d, %d, %d]", info, mouse_dx, mouse_dy);
		for (int i = 0; i < mouse_listeners_count; i++) {
			int (*listener)(mouse_device_packet_t) = mouse_listeners[i];
			if (!listener) continue;
			
			mouse_device_packet_t pkt = (mouse_device_packet_t) {
				.x_difference = mouse_dx,
				.y_difference = -mouse_dy,
				.x_sign = (info & 0b00001000) ? -1 : 1,
				.y_sign = (info & 0b00010000) ? -1 : 1,
				.x_delta_signed = mouse_dx,
				.y_delta_signed = mouse_dy,
				.buttons = 0,
			};
			listener(pkt);
		}
	}

	// if (!(mouse_byte[2] & 0x20)) {
	// 	y |= 0xFFFFFF00; //delta-y is a negative value
	// }

	// if (!(mouse_byte[2] & 0x10)) {
	// 	x |= 0xFFFFFF00; //delta-x is a negative value
	// }
}

void mouse_install() {
	log_info("MouseInstall", "Initializing PS/2 mouse interface");
	uint8_t status;
	idt_cli();

	// First, disable both keyboard and mouse devices
	mouse_wait(1);
	io_outb(0x64, 0xAD);  // Disable keyboard
	mouse_wait(1);
	io_outb(0x64, 0xA7);  // Disable mouse
	
	// Flush output buffer
	io_inb(0x60);

	// Get configuration byte
	mouse_wait(1);
	io_outb(0x64, 0x20);
	mouse_wait(0);
	status = io_inb(0x60);

	// Enable IRQ12 (mouse) and IRQ1 (keyboard)
	status |= (1 << 1);  // Enable IRQ12
	status |= (1 << 0);  // Enable IRQ1

	// Write back configuration
	mouse_wait(1);
	io_outb(0x64, 0x60);
	mouse_wait(1);
	io_outb(0x60, status);

	// Enable aux port
	mouse_wait(1);
	io_outb(0x64, 0xA8);

	// Ensure interrupts are enabled in the configuration
	mouse_wait(1);
	io_outb(0x64, 0x20);
	mouse_wait(0);
	status = io_inb(0x60);
	if (!(status & (1 << 1))) {
		log_info("MouseInstall", "Warning: Mouse IRQ not enabled in configuration");
	}

	// Rest of your initialization...
	// [Your existing sample rate setting code here]

	// setup sample rate
	// output sped 200, 200, 80
	mouse_write(0xF3);
	mouse_read();

	mouse_write(200);
	mouse_read();

	mouse_write(0xF3);
	mouse_read();

	mouse_write(200);
	mouse_read();

	mouse_write(0xF3);
	mouse_read();

	mouse_write(80);
	mouse_read();

	mouse_write(0xF6);
	mouse_read();
	mouse_write(0xF4);
	mouse_read();


	// Re-enable keyboard
	mouse_wait(1);
	io_outb(0x64, 0xAE);

	// Install handler and enable interrupts
	irq_install_handler(MOUSE_IRQ, mouse_handler);
}
