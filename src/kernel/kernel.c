#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "multiboot2.h"
#include "memory.h"
#include "fat_test.h"
#include "fat.h"
#include "arena.h"
#include "log.h"
#include "shell.h"
#include "../stdlib/printf.h"
#include "../stdlib/ctype.h"
#include "ui.h"
#include "rfos_splash.h"            // related to the splashbox
#include "tips.h"                   // related to the tipsbox
#include "files.h"
#include "ata.h"

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

// https://www.gnu.org/software/grub/manual/multiboot/html_node/Machine-state.html
// the first argument here should be the pointer to the mboot header
// the second argument should be a pointer to the returned data
static int i = 0;
void kernel_main(int magic, struct multiboot_header* header) {
    UNUSED(magic);
    ui_box_t splashbox, infobox, shellbox, tipsbox, filebox;

    serial_init();
    vga_init();

    log_info("Main", "Tags pointer: %x\n", header->tags);
    log_info("Main", "%x\n", magic);
    
    //log_info("DriveInfo", "%x\n", bootinfo->drives_addr);

    // Setup uiboxes
    splashbox = ui_new(0,  0,  39, 10, "Splash");
    ui_set_body_color(&splashbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
    ui_set_border_color(&splashbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);

    infobox = ui_new(40, 0,  24, 10, "Info");
    ui_set_body_color(&infobox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
    ui_set_border_color(&infobox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);

    tipsbox = ui_new(65, 0, 14, 10, "Tips");
    ui_set_body_color(&tipsbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_RED);
    ui_set_border_color(&tipsbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);

    shellbox = ui_new(0,  11, 59, 13, "Shell");
    ui_set_body_color(&shellbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
    ui_set_border_color(&shellbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);

    filebox = ui_new(60, 11, 19, 13, "Files");
    ui_set_body_color(&filebox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
    ui_set_border_color(&filebox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);

    // Display uiboxes
    ui_box(&splashbox);
    ui_box(&infobox);
    ui_box(&shellbox);
    ui_box(&tipsbox);
    ui_box(&filebox);
    rfos_splash(&splashbox);
    tips_populate(&tipsbox);
    files_populate(&filebox);

    //multiboot_debug(header, bootinfo);

    idt_cli();

    // Initialize hardware (sets up exception handlers as well)
    gdt_init();            ui_putstr(&infobox, 2, 0, "GDT      |Installed");
    idt_install();         ui_putstr(&infobox, 2, 1, "IDT      |Installed");

    // Setup irq handlers
    timer_init();          ui_putstr(&infobox, 2, 2, "Timer    |Init");
    keyboard_init();       ui_putstr(&infobox, 2, 3, "Keyboard |Init");
    mouse_init();          ui_putstr(&infobox, 2, 4, "Mouse    |Init (WIP)");

    // Initialize memory
    // memory_init(bootinfo); ui_putstr(&infobox, 2, 5, "Memory   |Init");
    ui_refresh();

    idt_sti();


    // Test ATA
    uint8_t buf[512];
    ata_select_drive(0xE0); // primary master
    ata_read(0, buf, 1);
    log_info("ATA", "%s\n", buf);// Check the last 2 bytes for the MBR signature
    for  (int i = 0; i < 512; i++) {
        serial_printf("%x ", buf[i]);
    }

    // Initialize fat filesystem
    /*if (bootinfo->mods_count > 0) {
        struct multiboot_module_s *mods = (struct multiboot_module_s*) bootinfo->mods_addr;
        fat_test(mods[0].mod_start);

        kernel_arena = arena_new(0x100000, 0x100000 + 0x7ee0000);

        // start reading the fat "drive"
        fat_init(mods[0].mod_start, mods[0].mod_end, &kernel_arena);
        fat_read_header();
        fat_debug();

        test_fat_read();
    }*/

    // ui_box_scroll_vertical(&splashbox);

    // Initialize shell stuff
    keyboard_add_listener(shell_keyboard_listener);
    // timer_add_listener(shell_timer_listener, 20);
    shell_run(&kernel_arena, &shellbox);

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
