#ifndef MALLOC
#define MALLOC malloc
#endif
#ifndef CALLOC
#define CALLOC calloc
#endif
#ifndef FREE
#define FREE free
#endif
#ifndef PRINTF
#define PRINTF printf
#endif
#ifndef LL_PRINT_PRE
#define LL_PRINT_PRE printf
#endif
#ifndef LL_PRINT_POST
#define LL_PRINT_POST printf
#endif

#define JOIN2(a, b) a ## b
#define JOIN3(a, b, c) a ## b ## c
#define JOIN4(a, b, c, d) a ## b ## c ## d
#define JOIN5(a, b, c, d, e) a ## b ## c ## d ## e

#define LL_MAIN_STRUCT_NAME(value_name) JOIN2(ll_, value_name)
#define LL_NODE_STRUCT_NAME(value_name) JOIN3(ll_, value_name, _node)

#define LL_STRUCT_DEF(value_name)\
	typedef struct LL_MAIN_STRUCT_NAME(value_name){\
		int size;\
		struct LL_NODE_STRUCT_NAME(value_name) *head, *tail;\
	} LL_MAIN_STRUCT_NAME(value_name);

#define LL_NODE_DEF(value_name)\
	typedef struct LL_NODE_STRUCT_NAME(value_name) {\
		value_name val;\
		struct LL_NODE_STRUCT_NAME(value_name) *next, *prev;\
	} LL_NODE_STRUCT_NAME(value_name);

#define LL_DEF(value_name)\
	LL_NODE_DEF(value_name)\
	LL_STRUCT_DEF(value_name)

#define LL_FREE_PROTO(value_name)\
	void JOIN3(ll_, value_name, _free)(LL_MAIN_STRUCT_NAME(value_name)*)\

#define LL_PUSH_BACK_PROTO(value_name)\
	void JOIN3(ll_, value_name, _pushback)(LL_MAIN_STRUCT_NAME(value_name)*, value_name)\

#define LL_PUSH_FRONT_PROTO(value_name)\
	void JOIN3(ll_, value_name, _pushfront)(LL_MAIN_STRUCT_NAME(value_name)*, value_name)\

#define LL_DELETE_PROTO(value_name)\
	void JOIN3(ll_, value_name, _delete)(LL_MAIN_STRUCT_NAME(value_name)*, value_name)\

#define LL_CONTAINS_PROTO(value_name)\
	bool JOIN3(ll_, value_name, _contains)(LL_MAIN_STRUCT_NAME(value_name)*, value_name)\

#define LL_CMP_VALUE_PROTO(value_name)\
	bool JOIN3(ll_, value_name, _cmp)(value_name, value_name)\

#define LL_PRINT_PROTO(value_name)\
	void JOIN3(ll_, value_name, _print_value)(value_name);\
	void JOIN3(ll_, value_name, _print)(LL_MAIN_STRUCT_NAME(value_name)*)\

#define LL_FREE_IMPL(value_name)\
	void JOIN3(ll_, value_name, _free)(LL_MAIN_STRUCT_NAME(value_name) *list) {\
		LL_NODE_STRUCT_NAME(value_name) *n = list->head;\
		while (n) {\
			LL_NODE_STRUCT_NAME(value_name) *t = n;\
			n = n->next;\
			FREE(t);\
		}\
	}

#define LL_PUSH_BACK_IMPL(value_name)\
	void JOIN3(ll_, value_name, _pushback)(LL_MAIN_STRUCT_NAME(value_name) *list, value_name val) {\
		if (list->head == NULL || list->tail == NULL) {\
			list->head = CALLOC(1, sizeof(LL_NODE_STRUCT_NAME(value_name)));\
			list->tail = list->head;\
			list->head->val = val;\
		}\
		else {\
			LL_NODE_STRUCT_NAME(value_name)* n = CALLOC(1, sizeof(*n));\
			n->val = val;\
			list->tail->next = n;\
			n->prev = list->tail;\
			list->tail = n;\
		}\
	}

#define LL_PUSH_FRONT_IMPL(value_name)\
	void JOIN3(ll_, value_name, _pushfront)(LL_MAIN_STRUCT_NAME(value_name) *list, value_name val) {\
		if (list->head == NULL || list->tail == NULL) {\
			list->head = CALLOC(1, sizeof(LL_NODE_STRUCT_NAME(value_name)));\
			list->tail = list->head;\
			list->head->val = val;\
		}\
		else {\
			LL_NODE_STRUCT_NAME(value_name)* n = CALLOC(1, sizeof(*n));\
			n->val = val;\
			list->head->prev = n;\
			n->next = list->head;\
			list->head = n;\
		}\
	}

#define LL_DELETE_IMPL(value_name)\
	void JOIN3(ll_, value_name, _delete)(LL_MAIN_STRUCT_NAME(value_name) *list, value_name val) {\
		if (list->head == NULL || list->tail == NULL) return;\
		if (list->head == list->tail && JOIN3(ll_, value_name, _cmp)(list->head->val, val)) {\
			FREE(list->head);\
			list->head = NULL;\
			list->tail = NULL;\
			return;\
		}\
		LL_NODE_STRUCT_NAME(value_name)* n = list->head;\
		while (n && (!JOIN3(ll_, value_name, _cmp)(n->val, val))) n = n->next;\
		if (n) {\
			LL_NODE_STRUCT_NAME(value_name)** prev = &n->prev;\
			LL_NODE_STRUCT_NAME(value_name)** next = &n->next;\
			(*prev)->next = (*next);\
			(*next)->prev = (*prev);\
			FREE(n);\
			return;\
		}\
	}\


#define LL_CONTAINS_IMPL(value_name)\
	bool JOIN3(ll_, value_name, _contains)(LL_MAIN_STRUCT_NAME(value_name) *list, value_name val) {\
		LL_NODE_STRUCT_NAME(value_name)* n = list->head;\
		while (n && JOIN3(ll_, value_name, _cmp)(n->val, val)) {\
			n = n->next;\
		}\
		if (n == NULL) return false;\
		return true;\
	}

#define LL_CMP_VALUE_IMPL(value_name)\
	bool JOIN3(ll_, value_name, _cmp)(value_name val1, value_name val2)

#define LL_PRINT_IMPL(value_name, connector)\
	void JOIN3(ll_, value_name, _print)(LL_MAIN_STRUCT_NAME(value_name) *list) {\
		LL_NODE_STRUCT_NAME(value_name)* n = list->head;\
		LL_PRINT_PRE("List\n");\
		while (n) {\
			PRINTF(LL_PRINT_FMT connector, LL_PRINT_ARGS);\
			/* JOIN3(ll_, value_name, _print_value)(n->val); */\
			/* PRINTF(connector); */\
			n = n->next;\
		}\
		PRINTF("NULL", "");\
		LL_PRINT_POST("List\n");\
	}\
	// void JOIN3(ll_, value_name, _print_value)(value_name val)
