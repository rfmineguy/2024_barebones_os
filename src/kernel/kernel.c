#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "timer.h"
#include "keyboard.h"
#include "multiboot.h"
#include "memory.h"
#include "fat_test.h"
#include "fat.h"
#include "arena.h"
#include "log.h"
#include "shell.h"
#include "../stdlib/printf.h"
#include "../stdlib/ctype.h"
#include "rfos_splash.h"

#define UNUSED(x) (void)(x)

arena kernel_arena;

void test_fat_read() {
    dir_entry* f = (void*)0;
    if (!fat_read_header())                  log_crit("FatRead", "Failed to read header\n"); 
    if (!fat_read())                         log_crit("FatRead", "Failed to read fat\n");
    if (!fat_read_root_dir())                log_crit("FatRead", "Failed to read root dir\n");
    if (!(f = fat_find_file("TEST    TXT"))) log_crit("FatRead", "Failed to find file\n");
    log_info("Main", "Found file \'TEST    TXT\'\n");
    log_info("Main", "Information about: Size %d, Name: %s, FirstClusterLow: %d, FirstClusterHigh: %d\n",
            f->Size, f->Name, f->FirstClusterLow, f->FirstClusterHigh);

    uint8_t* buf = fat_read_entry(f, &kernel_arena);
    if (!buf) log_info("Main", "Failed to read file\n");
    else {
        log_info("Main", "Success buf=%x\n", (uint32_t)buf);
        for (size_t i = 0; i < f->Size; i++) {
            if (isprint(buf[i])) serial_printf("%c", buf[i]);
            else serial_printf("<%X>", buf[i]);
        }
    }
}

static int i = 0;
void kernel_main(uint32_t magic, struct multiboot_info* bootinfo) {
    UNUSED(magic);
    ui_box splashbox, infobox, shell_box;
    splashbox = ui_box_new(0, 0, 29, 10);
    infobox = ui_box_new(50, 0, 29, 10);
    shell_box = ui_box_new(0, 11, 79, 13);


    serial_init();
    vga_init();

    multiboot_verify(magic, bootinfo);
    rfos_splash(&splashbox);

    idt_cli();

    ui_box_draw(&infobox);
    // Initialize hardware (sets up exception handlers as well)
    gdt_init();            ui_box_printf(&infobox, 0, 0, "Installed GDT");
    idt_install();         ui_box_printf(&infobox, 0, 1, "Installed IDT");

    // Setup irq handlers
    timer_init();          ui_box_printf(&infobox, 0, 2, "Initialized timer");
    keyboard_init();       ui_box_printf(&infobox, 0, 3, "Initialized keyboard");

    // Initialize memory
    memory_init(bootinfo); ui_box_printf(&infobox, 0, 4, "Initialized memory");

    idt_sti();
    //ui_box_printf(&infobox, "Everything initialized!\n");

    // Initialize fat filesystem
    if (bootinfo->mods_count > 0) {
        struct multiboot_module_s *mods = (struct multiboot_module_s*) bootinfo->mods_addr;
        fat_test(mods[0].mod_start);

        kernel_arena = arena_new(0x100000, 0x100000 + 0x7ee0000);

        // start reading the fat "drive"
        fat_init(mods[0].mod_start, mods[0].mod_end, &kernel_arena);
        fat_read_header();
        fat_debug();

        test_fat_read();
    }

    // Initialize shell stuff
    keyboard_add_listener(shell_keyboard_listener);
    shell_run(&kernel_arena, &shell_box);

    for(;;) {
        if (timer_ticks() % 20 == 0) {
            if (i % 3 == 0) vga_put_ch_at('-', 20, 20);
            if (i % 3 == 1) vga_put_ch_at('/', 20, 20);
            if (i % 3 == 2) vga_put_ch_at('|', 20, 20);
        }
        i++;
    }

    arena_free_all(&kernel_arena);
}
