#include "memory/frame_allocator.h"
#include "io/serial.h"
#include "memset.h"

uint8_t frame_bitmap[FRAME_BITMAP_SIZE];

void frame_init() {
  memset(frame_bitmap, 0, FRAME_BITMAP_SIZE);
}

/* allocate a physical memory frame
 *   searches for a free frame, and returns the physical address of that frame
 *
 * FRAME_BITMAP_SIZE - the number of bytes that represent the bitmap
 * FRAME_COUNT_MAX   - the max number of frames
 */
uint32_t frame_alloc() {
  for (int i = 0; i < FRAME_COUNT_MAX; i++) {
    int frame_bitmap_idx = i / 8;
    int frame_bitmap_bit = i % 8;

    if ((frame_bitmap[frame_bitmap_idx] & (0x1 << frame_bitmap_bit)) == 0) {
      frame_bitmap[frame_bitmap_idx] |= (0x1 << frame_bitmap_bit);
      return frame_bitmap_idx * 0x8000 + frame_bitmap_bit * 0x1000;
    }
  }
  return 0;
}

/* 
 * Free a physical memory frame given the address.
 * This doesnt require that the frame was previously allocated.
 *
 * NOTE: Im not sure if there is a need to check if the address is used or not
 * For now I just turn the bit off when this frame is requested to be freed
 */
void frame_free(uint32_t addr) {
  int frame_number = addr / FRAME_SIZE;
  int frame_bitmap_idx = frame_number / 8;
  int frame_bitmap_bit = frame_number % 8;
  int bitmask = ~(1 << frame_bitmap_bit);
  frame_bitmap[frame_bitmap_idx] &= bitmask;
}

/* Counts number of currently allocated frames
 */
uint32_t frame_count_allocated() {
  uint32_t count = 0;
  for (int i = 0; i < FRAME_COUNT_MAX; i++) {
    int frame_bitmap_idx = i / 8;
    int frame_bitmap_bit = i % 8;
    if ((frame_bitmap[frame_bitmap_idx] & (0x1 << frame_bitmap_bit)) != 0) {
      count++;
    }
  }
  return count;
}

#ifdef TEST_ENABLE
bool frame_is_used(uint32_t addr) {
  int frame_number = addr / FRAME_SIZE;
  int frame_bitmap_idx = frame_number / 8;
  int frame_bitmap_bit = frame_number % 8;
  return (frame_bitmap[frame_bitmap_idx] & (0x1 << frame_bitmap_bit)) != 0;
}

const uint8_t* frame_get_bitmap() {
  return &frame_bitmap;
}
#endif
