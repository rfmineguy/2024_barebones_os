#ifndef PAGING_H
#define PAGING_H
#include "../stdlib/stdint.h"

void paging_refresh();

void paging_init();

uint32_t paging_align_down(uint32_t addr, uint32_t page_size);
void     paging_map(uint32_t phys, uint32_t virt, uint32_t flags);
void     paging_map_range(uint32_t lo, uint32_t hi, uint32_t flags);

#endif
