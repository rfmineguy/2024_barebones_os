#ifndef MEMORY_H
#define MEMORY_H
#include "arch/x86/multiboot2.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct llist_node {
	bool free;
	void *begin, *end;
	struct llist_node *next, *prev;
} llist_node;

void memory_init(struct multiboot_header*);


void*    memory_alloc(uint32_t n);
void*    memory_calloc(uint32_t count, uint32_t n);
void     memory_free_int(void* n, const char* name);
void     memory_free(void* n);

void     memory_debug();

#endif
