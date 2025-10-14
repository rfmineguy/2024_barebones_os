#include "paging.h"
#include "log.h"
#include "../stdlib/stdint.h"
#include "../stdlib/limits.h"

#define PAGE_PRESENT 0x1

extern void paging_enable();
extern void paging_load_pagedir(uint32_t*);

uint32_t page_dir[1024] __attribute__((aligned(4096)));
uint32_t page_tables[1024][1024] __attribute__((aligned(4096)));

extern uint32_t kernel_start, kernel_end;

/*
 * ==================================
 * 0x4387
 * align(0x4387, 0x4000);
 *
 * diff = (0x0387) - 0x4000 
 * 			= -0x3613
 *      => (<0)
 *      => return 0x4387 - 0x0387;
 *
 * align(0x61cb88 + 0x1000, 0x1000);
 * diff = 0x0b88 - 0x1000
 * 			=> (<0)
 * 			=> return 0x61db88 - 0x0b88
 */
uint32_t paging_align_down(uint32_t addr, uint32_t page_size) {
	if (page_size == 0) return 0;
	return (addr - (addr % page_size)) & 0xfffffff000;
}

// must call paging_load_pagedir() after mapping address
// must specific PAGE_PRESENT(0x1) if enabling said page
void paging_map(uint32_t phys_addr, uint32_t virt_addr, uint32_t flags) {
	uint32_t pd_index = (virt_addr >> 22) & 0x03ff;
	uint32_t pt_index = (virt_addr >> 12) & 0x03ff;

	if (!(page_dir[pd_index] & PAGE_PRESENT)) {
		page_dir[pd_index] = ((uint32_t)&page_tables[pd_index][0]) | (flags & 0xfff);
	}
	page_tables[pd_index][pt_index] = (phys_addr & ~0xfff) | (flags & 0xfff);
}
