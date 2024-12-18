#define PRINTF log_line
#define LL_PRINT_PRE log_line_begin
#define LL_PRINT_POST log_line_end
#define LL_PRINT_FMT "%d"
#define LL_PRINT_ARGS n->val
#define MALLOC memory_alloc
#define CALLOC memory_calloc
#define FREE memory_free
#include "ll_int.h"
#include "log.h"
#include "memory.h"
#include "../printf.h"

void int_print(LL_NODE_STRUCT_NAME(int)* ll) {
	k_printf("{v=%d}", ll->val);
}

LL_FREE_IMPL(int);
LL_PRINT_IMPL(int, "->")
LL_CMP_VALUE_IMPL(int) {
	return val1 == val2;
}
LL_PUSH_BACK_IMPL(int);
LL_PUSH_FRONT_IMPL(int);
LL_DELETE_IMPL(int);
LL_CONTAINS_IMPL(int);
