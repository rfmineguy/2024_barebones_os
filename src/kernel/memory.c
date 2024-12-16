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

uint32_t memory_alloc(uint32_t byte_size) {
	//1. Find block big enough
	llist_node* n = root;
	while (n && !is_block_sufficient(n, byte_size)) {
		n = n->next;
	}

	//2. Split node into two, one that has the new allocation and one for the remaining free space
	//   Fixme: Implement freelist to encourage memory reuse
	llist_node* new = arena_alloc(&llist_arena, sizeof(llist_node));
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

void merge_free_nodes_rec(llist_node* n) {
	// Can't merge null node
	if (n == (void*)0) return;

	// Merge previous
	if (n->prev && n->prev->free) {
		n->begin = n->prev->begin;
		n->prev = n->prev->prev;
		n->prev->next = n;
		merge_free_nodes_rec(n);
		return;
	}

	// Merge next
	if (n->next && n->next->free){
		n->end = n->next->end;
		n->next = n->next->next;
		n->next->prev = n;
		merge_free_nodes_rec(n);
		return;
	}
}

void merge_free_nodes(llist_node* n) {
	llist_node *merge_start = n, *merge_end = n;
	while (merge_start && merge_start->free) merge_start = merge_start->prev;
	while (merge_end && merge_end->free) merge_end = merge_end->next;
	log_info("MergeFree", "Start = %x, End = %x", merge_start->begin, merge_end->end);
	// merge_start->end = merge_end->end;
	// merge_start->next = merge_end->next;
	// merge_end->next->prev = merge_start;

	// add dangling nodes to freelist
}

void memory_free(uint32_t addr) {
	// 1. Find allocation for addr
	llist_node* n = root;
	while (n) {
		if (n->begin == addr) break;
		n = n->next;
	}
	if (!n) {
		log_info("MemFree", "%x not allocated by memory_alloc", addr);
		return; // address not allocated by memory_alloc
	}

	// 2. Mark as free and merge neighbors
	n->free = true;
	merge_free_nodes_rec(n);
	log_info("MemFree", "%x", addr);

	// 3. Clear out region
	for (int i = malloc_arena.base + n->begin; i < malloc_arena.base + n->end; i++) {
		uint32_t* addr = (uint32_t*)(malloc_arena.base + i);
		*addr = 0;
	}
	log_info("MemFree", "Freed %x, size=%x", addr, n->end - n->begin);
}

void memory_debug() {
	llist_node* n = root;
	log_line_begin("Memory List");
	while (n) {
		log_line("{%s, %x - %x} <-> ", n->free ? "Free" : "Used", n->begin, n->end);
		n = n->next;
	}
	log_line_end("Memory List");
}
