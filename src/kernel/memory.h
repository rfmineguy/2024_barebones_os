#ifndef MEMORY_H
#define MEMORY_H
#include "multiboot2.h"
#include "../stdlib/stdint.h"
#include "../stdlib/stdbool.h"

typedef struct llist_node {
	bool free;
	uint32_t begin, end;
	struct llist_node *next, *prev;
} llist_node;

void memory_init(struct multiboot_header*);

uint32_t memory_alloc(uint32_t n);
void     memory_free(uint32_t n);

void     memory_debug();

#endif
