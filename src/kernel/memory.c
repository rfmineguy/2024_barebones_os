#include "memory.h"
#include "arena.h"
#include "printf.h"
#include "log.h"

llist_node* root = (void*)0;
llist_node* freelist = (void*)0;
arena llist_arena;
arena malloc_arena;

void memory_init(struct multiboot_header* info) {
	llist_arena  = arena_new(0x400000, 0x4fffff);
	malloc_arena = arena_new(0x500000, 0x5fffff);
	root = arena_alloc(&llist_arena, sizeof(llist_node));
	root->next = (void*)0;
	root->prev = (void*)0;
	root->begin = malloc_arena.base;
	root->end = malloc_arena.limit;
	root->free = true;

	freelist = (void*)0;
}

bool is_block_sufficient(llist_node* n, uint32_t byte_size) {
	return n->free && ((n->end - n->begin) >= byte_size);
}

void* memory_alloc(uint32_t byte_size) {
	//1. Find block big enough
	llist_node* n = root;
	while (n && !is_block_sufficient(n, byte_size)) {
		n = n->next;
	}
	log_info("MemAlloc", "Size: %x", byte_size);

	//2. Split node into two, one that has the new allocation and one for the remaining free space
	//   Fixme: Implement freelist to encourage memory reuse
	llist_node* new = 0;
	if (freelist != 0) {
		new = freelist;
		freelist = freelist->next;
	}
	else {
		new = arena_alloc(&llist_arena, sizeof(llist_node));
	}
	new->begin = n->begin;
	new->end = new->begin + byte_size;
	new->free = false;
	n->prev->next = new;
	new->next = n;
	n->begin = new->end + 1;

	// n = the free node to split
	if (n == root) {
		root = new;
		root->prev = 0;
		root->next = n;
		n->prev = root;
	}
	else {
		new->next = n;
		new->prev = n->prev;
		n->prev = new;
	}

	return new->begin;
}

void* memory_calloc(uint32_t count, uint32_t size) {
	void* p = memory_alloc(count * size);
	for (uint32_t i = 0; i < count * size; i ++) {
		*(char*)p = 0;
	}
	return p;
}

void merge_free_nodes_rec(llist_node* n) {
	// Can't merge null node
	if (n == (void*)0) return;

	// Merge previous
	if (n->prev && n->prev->free) {
		llist_node* to_free = n->prev;

		// Rewire allocation list
		n->begin = n->prev->begin;
		if (n->prev == root) root = n;
		else n->prev = n->prev->prev;
		n->prev->next = n;

		// Add to freelist
		llist_node* oldhead = freelist;
		llist_node** headp = &freelist;
		*headp = to_free;
		(*headp)->next = oldhead;
		oldhead->prev = *headp;

		merge_free_nodes_rec(n);
		return;
	}

	// Merge next
	if (n->next && n->next->free){
		llist_node* to_free = n->next;

		// Rewire allocation list
		n->end = n->next->end;
		n->next = n->next->next;
		n->next->prev = n;

		// Add to freelist
		llist_node* oldhead = freelist;
		llist_node** headp = &freelist;
		*headp = to_free;
		(*headp)->next = oldhead;
		oldhead->prev = *headp;

		merge_free_nodes_rec(n);
		return;
	}
}

void memory_free_int(void* addr, const char* name) {
	log_group_begin("MemFree");
	// 1. Find allocation for addr
	llist_node* n = root;
	while (n) {
		if (n->begin == addr) break;
		n = n->next;
	}
	if (!n) {
		log_crit("Error", "'%s' | %x not allocated by memory_alloc", name, addr);
		return; // address not allocated by memory_alloc
	}
	log_info("__", "'%s' | %x", name, addr);

	// 2. Mark as free and merge neighbors
	n->free = true;
	log_info("__", "Marked free");
	merge_free_nodes_rec(n);

	// 3. Clear out region
	for (char* i = malloc_arena.base + n->begin; i < malloc_arena.base + n->end; i++) {
		uint32_t* addr = (uint32_t*)(malloc_arena.base + i);
		*addr = 0;
	}
	log_info("MemFree", "Freed %x, size=%x", addr, n->end - n->begin);
	log_group_end("MemFree");
}

void memory_debug() {
	llist_node* n = root;
	log_group_begin("MemDebug");
		log_line_begin("Allocation List");
		while (n) {
			log_line("{%s, %x - %x} <-> ", n->free ? "Free" : "Used", n->begin, n->end);
			n = n->next;
		}
		log_line_end("Allocation List");
		log_line_begin("Free list");
		n = freelist;
		if (!n) {
			log_line("Empty");
		}
		else {
			while (n) {
				log_line("{Freed node} <-> ");
				n = n->next;
			}
		}
		log_line_end("Free list");
	log_group_end("MemDebug");
}
