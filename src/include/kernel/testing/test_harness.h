#ifndef TEST_HARNESS_H
#define TEST_HARNESS_H
#include "arch/x86/idt.h"

typedef enum {
  TEST_HARNESS_TYPE_NONE,
  TEST_HARNESS_TYPE_PAGE_FAULT,
  TEST_HARNESS_TYPE_DIV_ZERO_FAULT,
} test_harness_type;

typedef struct {
  test_harness_type type;
  struct interrupt_registers_test regs;
} test_harness_state;

void test_harness_trigger_divzero_fault(struct interrupt_registers_test);
void test_harness_trigger_page_fault(struct interrupt_registers_test);

test_harness_state test_harness_get_state();
test_harness_state test_harness_get_and_reset_state();

#endif
