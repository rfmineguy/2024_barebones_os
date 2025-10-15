#ifndef ARENA_H
#define ARENA_H
#include "../stdlib/stdint.h"

typedef struct arena {
    uint32_t base, limit; // these are to be treated as pointer values
    uint32_t used;        // this is how many bytes have been allocated
} arena;

// create a new arena that manages the memory 
// between base_addr, and limit_addr
//    [base_addr, limit_addr]
arena arena_new(uint32_t base_addr, uint32_t limit_addr);

// allocate a chunk of memory 'size' long, and return a
// pointer to the beginning of the allocation
// return NULL if its outside the [base_addr, limit_addr] range
void* arena_alloc(arena* arena, uint32_t size);

// set 'used' to 0, effectively freeing the arena
void  arena_free_all(arena* arena);

#endif
