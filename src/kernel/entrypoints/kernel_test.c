#include "testing/testing.h"
#include "arch/x86/multiboot2.h"
#include "printf.h"
#include "stdbool.h"

void kernel_main(int magic, struct multiboot_header* header) {
  (void)header;
  (void)magic;
  serial_init();

  rft_ctx ctx = {};

  rft_begin("rft", {
    rft_begin("assert_int", {
      rft_assert_int(1, ==, 1);
      rft_assert_int(1, !=, 2);
      rft_assert_int(1, <,  2);
      rft_assert_int(1, <=, 1);
      rft_assert_int(2, >,  1);
      rft_assert_int(1, >=, 1);
    })
    rft_begin("assert_true", {
      rft_assert_true(1);
      rft_assert_true(true);
    })
    rft_begin("assert_false", {
      rft_assert_false(0);
      rft_assert_false(false);
    })
  })
  rft_begin("printf", {
    rft_begin("return", {
      rft_assert_int(k_printf("hello"), ==, 5);
      rft_assert_int(k_printf(""), ==, 0);
      rft_assert_int(k_printf("%s %s", "hello", "world"), ==, 11);
    })
  })
  rft_report();

  for(;;);
}
