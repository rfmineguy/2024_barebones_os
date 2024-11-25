// This file is intended to be included in kernel.c when the KERNEL_TESTING
//  macro is defined
#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "serial.h"
#include "timer.h"
#include "keyboard.h"
#include "mouse.h"
#include "multiboot2.h"
#include "arena.h"
#include "log.h"
#include "shell.h"
#include "ui.h"
#include "rfos_splash.h"            // related to the splashbox
#include "tips.h"                   // related to the tipsbox
#include "files.h"                  // related to the filebox
#include "fat_drive.h"

#define UNUSED(x) (void)(x)

arena kernel_arena;

void kernel_main(int magic, struct multiboot_header* header) {
     UNUSED(magic);
     ui_box_t splashbox, infobox, shellbox, tipsbox, filebox;
 
     serial_init();
     vga_init();
 
     kernel_arena = arena_new(0x500000, 0x500000 + 0x7ee0000);
 
     // Drive setup
     log_group_begin("Drive Setup %s", "hi");
     fat_drive_read_header(); // read drive MBR
     fat_drive_debug_header();
     fat_drive_read(&kernel_arena);
     fat_drive_read_root_dir(&kernel_arena);
     log_group_end("Drive Setup");
 
     // Setup uiboxes
     splashbox = ui_new(0,  0,  37, 10, "Splash");
     ui_set_body_color(&splashbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
     ui_set_border_color(&splashbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);
 
     infobox = ui_new(38, 0,  17, 10, "Info");
     ui_set_body_color(&infobox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
     ui_set_border_color(&infobox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);
 
     tipsbox = ui_new(56, 0, 23, 10, "Tips");
     ui_set_body_color(&tipsbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
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
 
     idt_cli();
 
     // Initialize hardware (sets up exception handlers as well)
     gdt_init();            ui_putstr(&infobox, 2, 0, "GDT      [X]");
     idt_install();         ui_putstr(&infobox, 2, 1, "IDT      [X]");
 
     // Setup irq handlers
     timer_init();          ui_putstr(&infobox, 2, 2, "Timer    [X]");
     keyboard_init();       ui_putstr(&infobox, 2, 3, "Keyboard [X]");
     mouse_init();          ui_putstr(&infobox, 2, 4, "Mouse    [_]");
 
     // Initialize memory
     // memory_init(bootinfo); ui_putstr(&infobox, 2, 5, "Memory   |Init");
     ui_refresh();
 
     idt_sti();
 
     // Initialize shell stuff
     keyboard_add_listener(shell_keyboard_listener);
     // timer_add_listener(shell_timer_listener, 20);
     shell_run(&kernel_arena, &shellbox);
 
     for(;;) {
         // if (timer_ticks() % 20 == 0) {
         //     if (i % 3 == 0) vga_put_ch_at('-', 20, 20);
         //     if (i % 3 == 1) vga_put_ch_at('/', 20, 20);
         //     if (i % 3 == 2) vga_put_ch_at('|', 20, 20);
         // }
         // i++;
     }
 
     arena_free_all(&kernel_arena);
}
