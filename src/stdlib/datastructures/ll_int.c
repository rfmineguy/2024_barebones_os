#include "ll_int.h"
#include "../stdint.h"
#include "memory.h"
#include "../printf.h"

void int_print(LL_NODE_STRUCT_NAME(int)* ll) {
	k_printf("{v=%d}", ll->val);
}

LL_FREE_IMPL(int)
LL_PUSH_BACK_IMPL(int)
LL_PUSH_FRONT_IMPL(int)
LL_GETNODE_IMPL(int)
LL_DELETE_IMPL(int)
LL_PRINT_IMPL(int, int_print)
