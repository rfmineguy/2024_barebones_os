#include "arena.h"
#define LOG_DISABLE
#include "log.h"

arena arena_new(uint32_t base_addr, uint32_t limit_addr){
    log_info("ArenaNew", "created new arena at base_addr=%x, limit_addr=%x", 
            base_addr, limit_addr);
    return (arena) {.base=base_addr, .limit=limit_addr, .used=0};
}

void* arena_alloc(arena* arena, uint32_t size){
    if (arena->used + size > arena->limit) return (void*)0;
    void* ptr = (void*)(arena->base + arena->used);
    arena->used += size;
    log_info("ArenaAlloc", "allocated %d (0x%x) bytes at %x", size, size, ptr);
    return ptr;
}

void arena_free_all(arena* arena){
    arena->used = 0;
}
