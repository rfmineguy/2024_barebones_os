#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H
#include "stdint.h"
#include "limits.h"

#define FRAME_SIZE 4096
#define FRAME_COUNT_MAX (UINT_MAX / FRAME_SIZE)
#define FRAME_BITMAP_SIZE (UINT_MAX / FRAME_SIZE / 8)

void     frame_init();
uint32_t frame_alloc();
void     frame_free(uint32_t);
uint32_t frame_count_allocated();

#ifdef TEST_ENABLE
#include "stdbool.h"
bool  frame_is_used(uint32_t);
const uint8_t* frame_get_bitmap();
#endif

#endif
