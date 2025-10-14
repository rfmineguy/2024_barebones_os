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
