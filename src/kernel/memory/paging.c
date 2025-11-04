#include "memory/paging.h"
#include "io/log.h"
#include "stdint.h"
#include "limits.h"

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

// NOTE: Untested/Unused
uint32_t paging_get_page(uint32_t virt_addr) {
	uint32_t pd_index = (virt_addr >> 22) & 0x03ff;
	uint32_t pt_index = (virt_addr >> 12) & 0x03ff;
	return page_tables[pd_index][pt_index];
}

void paging_map_range(uint32_t lo, uint32_t hi, uint32_t flags) {
	uint32_t lo_aligned = paging_align_down(lo, 0x1000);
	uint32_t hi_aligned = paging_align_down(hi, 0x1000);
	log_info("PagingMapRange", "%x -> %x (%x -> %x)", lo, hi, lo_aligned, hi_aligned);

  uint32_t last_i = 0;
	for (uint32_t i = lo_aligned; i <= hi_aligned; i += 0x1000) {
    if ((int)last_i > (int)i) break;
		paging_map(i, i, flags);
    last_i = i;
	}
}

void paging_unmap_all() {
	for (int i = 0; i < 1024; i++) {
		for (int j = 0; j < 1024; j++) {
			page_tables[i][j] = 0; // present + r/w
		}
		page_dir[i] = 0;//((uint32_t)&page_tables[i]) | 0;
	}
}

void paging_map_kernel() {
	log_info("MapKernel", "%x - %x", (uint32_t)&kernel_start, (uint32_t)&kernel_end);
	paging_map_range((uint32_t)&kernel_start, (uint32_t)&kernel_end, 0x2 | PAGE_PRESENT);
}

void paging_map_vga() {
  // vga
  // each cell is 2 bytes, we have 80 columns and 24 rows
  // this works out to 2 * 80 * 24 bytes = 3840bytes
  // starting at 0xb8000

  paging_map_range(0xb8000, 0xb8000 + 0x1000, 0x2 | PAGE_PRESENT);
}

void paging_map_memory_allocator() {
  // memory allocator
  // identity maps arena ranges
  paging_map_range(0x400000, 0x600000, 0x2 | PAGE_PRESENT);
}

void paging_refresh() {
	paging_load_pagedir(page_dir);
}

void paging_init() {
	paging_unmap_all();
	paging_map_kernel();
	log_info("Paging", "Mapped kernel range");

  paging_map_vga();
  log_info("Paging", "Mapped vga");

  paging_map_memory_allocator();
  log_info("Paging", "Mapped memory allocator");

	paging_load_pagedir(page_dir);
	paging_enable();
	log_info("Paging", "Enabled paging");


	// uint32_t page = paging_get_page(0xc06573ac);
	// log_info("Page", "%x", page);

	uint32_t page = paging_get_page(0x0);
	log_info("Page", "%x", page);
	log_group_end("PagingInit");
}
