#include "testing/test_harness.h"

static test_harness_state state;

void test_harness_trigger_divzero_fault(struct interrupt_registers_test regs) {
  state.type = TEST_HARNESS_TYPE_DIV_ZERO_FAULT;
  state.regs = regs;
}

void test_harness_trigger_page_fault(struct interrupt_registers_test regs) {
  state.type = TEST_HARNESS_TYPE_PAGE_FAULT;
  state.regs = regs;
}

test_harness_state test_harness_get_state() {
  return state;
}

test_harness_state test_harness_get_and_reset_state() {
  test_harness_state old = state;
  state = (test_harness_state){0};
  return old;
}
