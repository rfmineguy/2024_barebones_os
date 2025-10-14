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
