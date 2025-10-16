#ifndef TESTING_H
#define TESTING_H
#include "io/log.h"
#include "string.h"

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define CYAN "\x1b[36m"

#define PURPLE_BOLD "\x1b[1;35m"
#define DEF "\x1b[0m"

#define COLORED(color, text) color text DEF

#ifndef TESTPATH_LENGTH
#define TESTPATH_LENGTH 30
#endif

#ifndef ASSERT_CONTENT_LENGTH
#define ASSERT_CONTENT_LENGTH 40
#endif

#ifndef MAX_GROUP_ASSERTION_COUNT
#define MAX_GROUP_ASSERTION_COUNT 100
#endif

#ifndef MAX_GROUP_STACK_SIZE
#define MAX_GROUP_STACK_SIZE 100
#endif

#define rft_report() {\
  serial_printf(CYAN "%d of %d passed\n" DEF, ctx.passing, ctx.total);\
}

#define rft_begin(label_, block)\
  {\
    ctx.group_stack[ctx.groups_count++] = label_;\
    block\
    ctx.groups_count--;\
  }

#define rft_print_groupstring()\
{\
  int current_line_length = 0;\
  for (int i = 0; i < ctx.groups_count; i++) {\
    serial_write_str((char*)ctx.group_stack[i]);\
    serial_write_ch('/');\
    current_line_length += strlen(ctx.group_stack[i]) + 1;\
  }\
  if (current_line_length < TESTPATH_LENGTH) {\
    for (int i = 0; i < TESTPATH_LENGTH - current_line_length; i++) {\
      serial_write_ch(' ');\
    }\
  }\
}

#define rft_print_assert_content(fmt, ...)\
{\
  serial_write_str(COLORED(CYAN, "["));\
  int current_line_length = serial_printf(fmt, __VA_ARGS__);\
  for (int i = 0; i < ASSERT_CONTENT_LENGTH - current_line_length; i++) {\
    serial_write_ch(' ');\
  }\
  serial_write_str(COLORED(CYAN, "]"));\
}\

#define rft_pass(assert_string, ...)\
  rft_print_groupstring()\
  rft_print_assert_content(assert_string, __VA_ARGS__);\
  serial_printf(COLORED(GREEN, "[OK  ]") "\n");

#define rft_fail(assert_string, ...)\
  rft_print_groupstring()\
  rft_print_assert_content(assert_string, __VA_ARGS__);\
  serial_printf(COLORED(RED, "[FAIL]") "\n");

#define rft_assert_int(a, op, b) {\
  if (!((a) op (b))) { \
    rft_fail(COLORED(PURPLE_BOLD, "%s") COLORED(CYAN, "%s") COLORED(PURPLE_BOLD, "%s"), #a, #op, #b);\
  }\
  else { \
    rft_pass(COLORED(PURPLE_BOLD, "%s") COLORED(CYAN, "%s") COLORED(PURPLE_BOLD, "%s"), #a, #op, #b);\
    ctx.passing += 1;\
  }\
  ctx.total += 1;\
}

#define rft_assert_true(a) {\
  if (!(a)) { rft_fail(COLORED(PURPLE_BOLD, "%s"), #a); }\
  else { rft_pass(COLORED(PURPLE_BOLD, "%s"), #a); ctx.passing += 1; }\
  ctx.total += 1;\
}

#define rft_assert_false(a) {\
  if ((a)) { rft_fail(COLORED(PURPLE_BOLD, "%s"), #a); }\
  else { rft_pass(COLORED(PURPLE_BOLD, "%s"), #a); ctx.passing += 1; }\
  ctx.total += 1;\
}

typedef struct {
  const char* assert_string;
} rft_assert;

typedef struct {
  int passing, total;
  const char* group_stack[MAX_GROUP_STACK_SIZE];
  int groups_count;
} rft_ctx;

typedef enum {
  OK, ERROR
} rft_result;

#endif
