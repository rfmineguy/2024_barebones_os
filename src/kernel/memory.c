#include "memory.h"
#include "arena.h"
#include "printf.h"
#include "log.h"
#include "linkedlist_memory_node.h"

arena llist_arena;
arena malloc_arena;
ll_memory_node memory_list;

void* allocator_malloc(int size) {
  return 0;
}
void* allocator_calloc(int count, int size) {
  return arena_alloc(&llist_arena, count * size);
}
void allocator_free(void* ptr) {
}

void memory_init(struct multiboot_header* info) {

  memory_list = ll_memory_node_create_w_allocator((allocator){
    .malloc = allocator_malloc,      // normal malloc not used, strictly calloc
    .calloc = allocator_calloc,
    .free = allocator_free,
  });

	llist_arena  = arena_new(0x400000, 0x4fffff);
	malloc_arena = arena_new(0x500000, 0x5fffff);

  ll_memory_node_pushfront(&memory_list, (memory_node) {.begin = (void*)malloc_arena.base, .end = (void*)malloc_arena.limit, .free = true});
}

bool is_block_sufficient(memory_node n, uint32_t byte_size) {
	return n.free && ((uint32_t)(n.end - n.begin) >= byte_size);
}

void* memory_alloc(uint32_t byte_size) {
	//1. Find block big enough
  ll_memory_node_node* n = memory_list.head;
  while (n && !is_block_sufficient(n->val, byte_size)) n = n->next;
  if (!n) {
    return NULL; // failed to find block big enough
  }

  //2. Create a new memory node representing the new node
  memory_node newmem = {
    .begin = n->val.begin,
    .end = n->val.begin + byte_size,
    .free = false,
  };
  ll_memory_node_insert_after(&memory_list, n, newmem);

  //3. Modify the split node to reflect its new free size
  n->val.begin = newmem.end + 1;

	return newmem.begin;
}

void* memory_calloc(uint32_t count, uint32_t size) {
	void* p = memory_alloc(count * size);
  if (!p) return NULL;
	for (uint32_t i = 0; i < count * size; i ++) {
		*(char*)p = 0;
	}
	return p;
}

void merge_free_nodes_rec(ll_memory_node_node* n) {
	// Can't merge null node
	if (n == (void*)0) return;

	// Merge previous
	if (n->prev && n->prev->val.free) {
		ll_memory_node_node* to_free = n->prev;

		// Rewire allocation list
		n->val.begin = n->prev->val.begin;
		if (n->prev == memory_list.head) memory_list.head = n;
		else n->prev = n->prev->prev;
		n->prev->next = n;

		// Add to freelist
    ll_memory_node_helper_add_to_freelist(&memory_list, to_free);
		//ll_memory_node_node* oldhead = memory_list.freelist;
		//ll_memory_node_node** headp = &memory_list.freelist;
		//*headp = to_free;
		//(*headp)->next = oldhead;
		//oldhead->prev = *headp;

		merge_free_nodes_rec(n);
		return;
	}

	// Merge next
	if (n->next && n->next->val.free){
		ll_memory_node_node* to_free = n->next;

		// Rewire allocation list
		n->val.end = n->next->val.end;
		n->next = n->next->next;
		n->next->prev = n;

		// Add to freelist
		ll_memory_node_node* oldhead = memory_list.freelist;
		ll_memory_node_node** headp = &memory_list.freelist;
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
	ll_memory_node_node* n = memory_list.head;
	while (n) {
		if (n->val.begin == addr) break;
		n = n->next;
	}
	if (!n) {
		log_crit("Error", "'%s' | %x not allocated by memory_alloc", name, addr);
		log_group_end("MemFree");
		return; // address not allocated by memory_alloc
	}
	log_info("__", "'%s' | %x", name, addr);

	// 2. Mark as free and merge neighbors
	n->val.free = true;
	log_info("__", "Marked free");
	merge_free_nodes_rec(n);

	// 3. Clear out region
	log_info("MemFree", "Freed %x, size=%x", addr, n->val.end - n->val.begin);
	log_group_end("MemFree");
}

void memory_free(void* n) {
  memory_free_int(n, ".");
}

void memory_debug() {
  ll_memory_node_node* n = memory_list.head;
	log_group_begin("MemDebug");
		log_line_begin("Allocation List");
		while (n) {
			log_line("{%s, %x - %x} <-> ", n->val.free ? "Free" : "Used", n->val.begin, n->val.end);
			n = n->next;
		}
		log_line_end("Allocation List");
		log_line_begin("Free list");
		n = memory_list.freelist;
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
