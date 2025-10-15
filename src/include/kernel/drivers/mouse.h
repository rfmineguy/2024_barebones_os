#ifndef MOUSE2_H
#define MOUSE2_H
#include "../stdlib/stdint.h"

typedef enum {
	LEFT_CLICK   = 0x01,
	RIGHT_CLICK  = 0x02,
	MIDDLE_CLICK = 0x04
} mouse_click_t;

typedef struct {
	uint32_t magic;
	int8_t x_difference;
	int8_t y_difference;
	int8_t x_sign;
	int8_t y_sign;

	int8_t x_delta_signed;
	int8_t y_delta_signed;

	mouse_click_t buttons;
} mouse_device_packet_t;

#define MOUSE_MAGIC 0xFEED1234

void mouse_add_listener(int(*)(mouse_device_packet_t));
void mouse_install();

#endif

