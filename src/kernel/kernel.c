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
#include "shell_2.h"
#include "ui.h"
#include "cpu.h"
#include "rfos_splash.h"            // related to the splashbox
#include "tips.h"                   // related to the tipsbox
#include "files.h"                  // related to the filebox
#include "fat_drive.h"
#include "event_system.h"
#include "memory.h"
#include "paging.h"
#include "datastructures/ll_int.h"
#include "linkedlist_memory_node.h"

#define UNUSED(x) (void)(x)

arena kernel_arena;

void kernel_main(int magic, struct multiboot_header* header) {
     UNUSED(magic);
     ui_box_t splashbox, infobox, shellbox, tipsbox, filebox;
		 ui_box_t mainbox;
 
     serial_init();
     vga_init();
     vga_writestring("afkhsd");
 
     idt_cli();
 
     gdt_init();
     idt_install();
     timer_init();
     keyboard_init();
     mouse_install();

		 log_info("Kernel", "Setup GDT, IDT, Timer, Keyboard, Mouse");

		 // idt_debug_setup();
 
     idt_sti();
		 log_info("Interrupts", "Enabled");

     paging_init();
     memory_init(header);



     // Drive setup
     log_group_begin("Drive Setup %s", "hi");
     fat_drive_read_header(); // read drive MBR
     fat_drive_debug_header();
     fat_drive_read();
     fat_drive_read_root_dir();
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

     // // Display uiboxes
     ui_box(&splashbox);
     ui_box(&infobox);
     ui_box(&shellbox);
     ui_box(&tipsbox);
     ui_box(&filebox);
     // rfos_splash(&splashbox);
     // tips_populate(&tipsbox);
     // files_populate(&filebox);

		 mainbox = ui_new(0, 0, 79, 24, "Main");
     ui_set_body_color(&mainbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLUE);
     ui_set_border_color(&mainbox, VGA_COLOR_LIGHT_GREY, VGA_COLOR_LIGHT_BLUE);
		 log_info("Kernel", "Setup mainbox");
 
     shell2_run(&mainbox);

		 log_info("CPU", "Halting");
		 cpu_halt();
}
