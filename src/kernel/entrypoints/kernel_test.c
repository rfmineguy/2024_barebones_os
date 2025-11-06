#define ASSERT_CONTENT_LENGTH 50
#define TEST_ENABLE
#include "testing/testing.h"
#include "testing/test_harness.h"
#include "arch/x86/multiboot2.h"
#include "arch/x86/gdt.h"
#include "arch/x86/idt.h"
#include "memory/paging.h"
#include "memory/arena.h"
#include "memory/frame_allocator.h"
#include "event/event_system.h"
#include "printf.h"
#include "stdbool.h"

/**
 *  This test main will still initialize all the normal things the normal main
 *  kernel file initializes, and in the same order.
 *
 *  The only difference is that we are testing the actual result of that initialization
 *  to make sure it happened correctly, and expectedly
 */
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
    rft_begin("assert_null", {
      rft_assert_null(0);
      rft_assert_null(NULL);
    })
    rft_begin("assert_nonnull", {
      rft_assert_non_null(1);
      rft_assert_non_null(100000000);
    })
  })
  rft_begin("printf", {
    rft_begin("return", {
      rft_assert_int(k_printf("hello"), ==, 5);
      rft_assert_int(k_printf(""), ==, 0);
      rft_assert_int(k_printf("%s %s", "hello", "world"), ==, 11);
    })
  })
  idt_cli();
  gdt_init();
  rft_mark("initialized the gdt");
  rft_begin("gdt", {
    rft_begin("structure", {
      rft_begin("entry_0", {
        const struct gdt_entry* e = gdt_get_entry(0);
        rft_assert_int(e->access, ==, 0x0);
        rft_assert_int(e->flags_limit, ==, 0x0);
      })
      rft_begin("entry_1", {
        const struct gdt_entry* e = gdt_get_entry(1);
        rft_assert_int(e->access, ==, 0x9A);
        rft_assert_int(e->flags_limit & 0x0f, ==, (0xffffffff >> 16) & 0x0f);
      })
      rft_begin("entry_2", {
        const struct gdt_entry* e = gdt_get_entry(2);
        rft_assert_int(e->access, ==, 0x92);
        rft_assert_int(e->flags_limit & 0x0f, ==, (0xffffffff >> 16) & 0x0f);
      })
      rft_begin("entry_3", {
        const struct gdt_entry* e = gdt_get_entry(3);
        rft_assert_int(e->access, ==, 0xFA);
        rft_assert_int(e->flags_limit & 0x0f, ==, (0xffffffff >> 16) & 0x0f);
      })
      rft_begin("entry_4", {
        const struct gdt_entry* e = gdt_get_entry(4);
        rft_assert_int(e->access, ==, 0xF2);
        rft_assert_int(e->flags_limit & 0x0f, ==, (0xffffffff >> 16) & 0x0f);
      })
      // entry_5 is the tss entry. not sure how to test that yet
      rft_begin("entry_6", {
        rft_mark("entry_6 should not exist");
        rft_assert_int(gdt_get_entry(6), ==, NULL);
      })
    })
  })

  idt_install();
  rft_mark("initialized interrupts");

  paging_init();
  rft_mark("Initialized paging and identity mapped some intial paged");
  rft_begin("memory", {
    rft_begin("paging", {
      rft_begin("align", {
        rft_assert_int(paging_align_down(0x45372, 0x4000), ==, 0x44000);
        rft_assert_int(paging_align_down(0x49999, 0x4000), ==, 0x48000);
        rft_assert_int(paging_align_down(0x0,     0x4000), ==, 0x0);
        rft_assert_int(paging_align_down(0x500,   0x4000), ==, 0x0);
        rft_assert_int(paging_align_down(0x1,     0x4000), ==, 0x0);
      })
      rft_begin("mapping", {
        rft_mark("Mapping tests");
        rft_assert_int(paging_get_page(0x0) & PAGE_PRESENT, ==, 0);
        rft_assert_int(paging_get_page(0x400000) & PAGE_PRESENT, ==, 1);
      })
      // paging_map(0x0, 0x0, 0x2 | PAGE_PRESENT);
      rft_begin("fault_handling", {
        rft_mark("Write to unmapped memory");
        *((int*)1) = 3;   // cause page fault
        test_harness_state state = test_harness_get_and_reset_state();
        rft_assert_int(state.type,     ==, TEST_HARNESS_TYPE_PAGE_FAULT);
        rft_assert_int(state.regs.cr2, ==, 1);

        // this page should be allocated now
        rft_mark("Write to newly mapped memory");
        *((int*)1) = 3;
        state = test_harness_get_and_reset_state();
        rft_assert_int(state.type,     ==, TEST_HARNESS_TYPE_NONE);

        // cause page fault
        rft_mark("Write to unmapped memory");
        *((int*)0x5674) = 3;
        state = test_harness_get_and_reset_state();
        rft_assert_int(state.type,     ==, TEST_HARNESS_TYPE_PAGE_FAULT);
        rft_assert_int(state.regs.cr2, ==, 0x5674);

        // this page should be allocated now
        rft_mark("Write to newly mapped memory");
        *((int*)0x5674) = 3;
        state = test_harness_get_and_reset_state();
        rft_assert_int(state.type,     ==, TEST_HARNESS_TYPE_NONE);
      })
    })
  })
  rft_begin("datastructures", {
    rft_begin("linkedlist", {
      rft_mark("Linked List");
    })
  })
  rft_begin("eventsystem", {
    rft_begin("init", {
      event_system_init();
      const queue_event* e = event_system_get_event_queue();
      rft_assert_int(e->size, ==, 0);
    })
    rft_begin("post & get", {
      const queue_event* e = event_system_get_event_queue();

      event_system_post((event) {.type = KEYBOARD, .keyboard = {.ch = 'a', .press = 'a', .modifier_flags = L_CONTROL}});
      rft_assert_int(e->size, ==, 1);

      event event;
      event_system_get(&event);
      rft_assert_int(event.type, ==, KEYBOARD);
      rft_assert_int(event.keyboard.ch, ==, 'a');
      rft_assert_int(event.keyboard.press, ==, 'a');
      rft_assert_int(event.keyboard.modifier_flags, ==, L_CONTROL);

      rft_mark("Make sure that getting the event removes it from the queue");
      rft_assert_int(e->size, ==, 0);
      event_system_deinit();
    })
  })
  rft_begin("arena", {
    arena a = arena_new(0x0, 0x4000);
    rft_begin("new", {
      rft_assert_int(a.base, ==, 0x0);
      rft_assert_int(a.limit, ==, 0x4000);
      rft_assert_int(a.used, ==, 0);
    })
    rft_begin("alloc", {
      rft_mark("Allocation (500 bytes)");
      void* p = arena_alloc(&a, 500);
      rft_assert_int(p, ==, (void*)a.base);
      rft_assert_int(a.base, ==, 0x0);
      rft_assert_int(a.limit, ==, 0x4000);
      rft_assert_int(a.used, ==, 500);

      rft_mark("Allocation (1500 bytes)");
      p = arena_alloc(&a, 1500);
      rft_assert_int(p, ==, (void*)a.base + 500);
      rft_assert_int(a.base, ==, 0x0);
      rft_assert_int(a.limit, ==, 0x4000);
      rft_assert_int(a.used, ==, 2000);
    })
    rft_begin("free", {
      arena_free_all(&a);
      rft_assert_int(a.base, ==, 0x0);
      rft_assert_int(a.limit, ==, 0x4000);
      rft_assert_int(a.used, ==, 0);
    })
  })
  rft_begin("frame_allocator", {
    rft_begin("frame_init", {
      frame_init();
      rft_assert_mem_zeroed(frame_get_bitmap(), FRAME_BITMAP_SIZE);
    })

    rft_begin("alloc/free", {
      uint32_t frame = frame_alloc();
      rft_assert_int(frame, ==, 0x0);
      rft_assert_true(frame_is_used(0x0));
      rft_assert_int(frame_count_allocated(), ==, 1);

      frame = frame_alloc();
      rft_assert_int(frame, ==, 0x1000);
      rft_assert_true(frame_is_used(0x1000));
      rft_assert_false(frame_is_used(0x2000));

      rft_assert_int(frame_count_allocated(), ==, 2);

      frame_free(0x1000);
      rft_assert_false(frame_is_used(0x1000));
      rft_assert_int(frame_count_allocated(), ==, 1);

      frame_free(0x0);
      rft_assert_false(frame_is_used(0x0));
      rft_assert_int(frame_count_allocated(), ==, 0);
    })
  })
  rft_report();

  for(;;);
}
