#ifndef LINKED_LIST_TPLT_H
#define LINKED_LIST_TPLT_H

#define JOIN2(a, b) a ## b
#define JOIN3(a, b, c) a ## b ## c

/* Structure name macros */
#define LL_NODE_STRUCT_NAME(type) JOIN3(ll_, type, _node_t)
#define LL_STRUCT_NAME(type) JOIN3(ll_, type, _t)

/* Structure definition macros */
/* Linked list node struct type definition */
#define LL_NODE_DEF(type)\
	typedef struct LL_NODE_STRUCT_NAME(type) {\
		type val;\
		struct LL_NODE_STRUCT_NAME(type) *next, *prev;\
	} LL_NODE_STRUCT_NAME(type)

#define LL_STRUCT_DEF(type)\
	typedef struct LL_STRUCT_NAME(type) {\
		LL_NODE_STRUCT_NAME(type) *head, *tail;\
	} LL_STRUCT_NAME(type)

/* Function macros */
/*    + Free
 *    + PushBack
 *    + PushFront
 *    + GetNode
 *    + Delete
 *    + Print
 */

#define RET(type) LL_NODE_STRUCT_NAME(type)
#define LL_FREE_PROTO(type) 			void 			 JOIN2(ll_free_, type)(LL_STRUCT_NAME(type)*)
#define LL_PUSH_BACK_PROTO(type)  void 	  	 JOIN2(ll_pushback_, type)(LL_STRUCT_NAME(type)*, type)
#define LL_PUSH_FRONT_PROTO(type) void 		   JOIN2(ll_pushfront_, type)(LL_STRUCT_NAME(type)*, type)
#define LL_GETNODE_PROTO(type)    RET(type)* JOIN2(ll_getnode_, type)(LL_STRUCT_NAME(type)*, type)
#define LL_DELETE_PROTO(type)     void 			 JOIN2(ll_delete_, type)(LL_STRUCT_NAME(type)*, type)
#define LL_PRINT_PROTO(type) 			void 			 JOIN2(ll_print_, type)(LL_STRUCT_NAME(type)*)

#define LL_FREE_IMPL(type) 		    void 			 JOIN2(ll_free_, type)(LL_STRUCT_NAME(type) * ll) {\
	struct LL_NODE_STRUCT_NAME(type) *n = ll->head;\
	while (n) {\
		struct LL_NODE_STRUCT_NAME(type) *t = n;\
		n = n->next;\
		memory_free((uint32_t)t);\
	}\
}

#define LL_PUSH_BACK_IMPL(type)   void   	   JOIN2(ll_pushback_, type)(LL_STRUCT_NAME(type) * ll, type v) {\
	struct LL_NODE_STRUCT_NAME(type) *n = (struct LL_NODE_STRUCT_NAME(type)*)memory_alloc(sizeof(LL_NODE_STRUCT_NAME(type)));\
	n->val = v;\
	/* Check allocation */\
	if (n == (void*)0) return;\
	/* First push */ \
	if (ll->tail == 0 && ll->head == 0) {\
		ll->head = n;\
		ll->tail = ll->head;\
		ll->head->prev = 0;\
		ll->head->next = 0;\
		return;\
	}\
	/* Second push */\
	/*   Push to the tail */\
	if (ll->head == ll->tail) {\
		ll->tail = n;\
		ll->tail->next = 0;\
		ll->tail->prev = ll->head;\
		ll->head->next = n;\
		n->prev = ll->head;\
		return;\
	}\
	/* All other pushes */\
	n->prev = ll->tail;\
	ll->tail->next = n;\
	ll->tail = n;\
	ll->tail->next = 0;\
}
#define LL_PUSH_FRONT_IMPL(type)  void   	   JOIN2(ll_pushfront_, type)(LL_STRUCT_NAME(type) * ll, type v) {\
	struct LL_NODE_STRUCT_NAME(type) *n = (struct LL_NODE_STRUCT_NAME(type)*)memory_alloc(sizeof(LL_NODE_STRUCT_NAME(type)));\
	n->val = v;\
	/* Check allocation */\
	if (n == (void*)0) return;\
	/* First push */ \
	if (ll->tail == 0 && ll->head == 0) {\
		ll->head = n;\
		ll->tail = ll->head;\
		ll->head->prev = 0;\
		ll->head->next = 0;\
		return;\
	}\
	/* Second push */\
	/*   Push to the head */\
	if (ll->head == ll->tail) {\
		ll->head = n;\
		ll->head->next = ll->tail;\
		ll->head->prev = 0;\
		ll->tail->prev = n;\
		n->next = ll->tail;\
		return;\
	}\
	/* All other pushes */\
	n->next = ll->head;\
	ll->head->prev = n;\
	ll->head = n;\
	ll->head->prev = 0;\
}
#define LL_GETNODE_IMPL(type)     RET(type)* JOIN2(ll_getnode_, type)(LL_STRUCT_NAME(type) * ll, type v) {\
	struct LL_NODE_STRUCT_NAME(type) *n = ll->head;\
	while (n && n->val != v) n = n->next;\
	return n;\
}
#define LL_DELETE_IMPL(type) void JOIN2(ll_delete_, type)(LL_STRUCT_NAME(type) * ll, type v) {\
	(void)ll;\
	(void)v;\
}
#define LL_PRINT_IMPL(type, printfunc) void JOIN2(ll_print_, type)(LL_STRUCT_NAME(type) * ll) {\
	struct LL_NODE_STRUCT_NAME(type) *n = ll->head;\
	while (n) {\
		printfunc(n);\
		k_printf(" <-> ");\
		n = n->next;\
	}\
}

#endif
