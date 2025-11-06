#include "memory/frame_allocator.h"
#include "io/serial.h"
#include "memset.h"

uint8_t frame_bitmap[FRAME_BITMAP_SIZE];

void frame_init() {
  memset(frame_bitmap, 0, FRAME_BITMAP_SIZE);
}
