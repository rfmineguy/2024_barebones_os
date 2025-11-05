#include "arch/x86/multiboot2_helper.h"
#include "io/log.h"
#include "stdint.h"

static void mboot2_helper_print_tag_mmap(struct multiboot_tag_mmap* mmap) {
  log_info("MBoot Tag Memory Map", "type         =%x", mmap->type);
  log_info("MBoot Tag Memory Map", "size         =%x", mmap->size);
  log_info("MBoot Tag Memory Map", "entry_version=%x", mmap->entry_version);
  log_info("MBoot Tag Memory Map", "entry_size   =%x", mmap->entry_size);
}

static void mboot2_helper_print_tag_apm(struct multiboot_tag_apm* apm) {
  log_info("MBoot Tag APM", "cseg       =%x", apm->cseg);
  log_info("MBoot Tag APM", "cseg_16    =%x", apm->cseg_16);
  log_info("MBoot Tag APM", "cseg_16_len=%x", apm->cseg_16_len);
  log_info("MBoot Tag APM", "cseg_len   =%x", apm->cseg_len);
  log_info("MBoot Tag APM", "dseg       =%x", apm->dseg);
  log_info("MBoot Tag APM", "dseg_len   =%x", apm->dseg_len);
  log_info("MBoot Tag APM", "flags      =%x", apm->flags);
  log_info("MBoot Tag APM", "offset     =%x", apm->offset);
  log_info("MBoot Tag APM", "version    =%x", apm->version);
}

static void mboot2_helper_print_tag_bootloader_name(struct multiboot_tag_string* string) {
  log_info("MBoot Tag Bootloader Name", "name=%s", string->string);
}

static void mboot2_helper_print_tag_cmdline(struct multiboot_tag_string* string) {
  log_info("MBoot Tag Cmdline", "string=%s", string->string);
}

static void mboot2_helper_print_tag_acpi_new(struct multiboot_tag_new_acpi* acpi) {
  log_info("MBoot Acpi New", "<unknown>");
}

static void mboot2_helper_print_tag_acpi_old(struct multiboot_tag_old_acpi* acpi) {
  log_info("MBoot Acpi Old", "<unknown>");
}

static void mboot2_helper_print_tag_basic_meminfo(struct multiboot_tag_basic_meminfo* meminfo) {
  log_info("MBoot Basic MemInfo", "mem_lower=%x", meminfo->mem_lower);
  log_info("MBoot Basic MemInfo", "mem_upper=%x", meminfo->mem_upper);
}

static void mboot2_helper_print_tag_bootdev(struct multiboot_tag_bootdev* bootdev) {
  log_info("MBoot Bootdev", "biosdev=%x", bootdev->biosdev);
  log_info("MBoot Bootdev", "part=%x", bootdev->part);
  log_info("MBoot Bootdev", "slice=%x", bootdev->slice);
}

static void mboot2_helper_print_tag_framebuffer(struct multiboot_tag_framebuffer* framebuffer) {
  log_group_begin("MBoot FrameBuffer");
  log_info("Common", "reserved=%x", framebuffer->common.reserved);
  log_info("Common", "framebuffer_addr=%x", framebuffer->common.framebuffer_addr);
  log_info("Common", "framebuffer_bpp=%x", framebuffer->common.framebuffer_bpp);
  log_info("Common", "framebuffer_width=%x", framebuffer->common.framebuffer_width);
  log_info("Common", "framebuffer_height=%x", framebuffer->common.framebuffer_height);
  log_info("Common", "framebuffer_pitch=%x", framebuffer->common.framebuffer_pitch);
  log_group_end("MBoot FrameBuffer");
  log_info("MBoot Bootdev", "framebuffer_blue_field_position=%x", framebuffer->framebuffer_blue_field_position);
  log_info("MBoot Bootdev", "framebuffer_blue_mask_size=%x", framebuffer->framebuffer_blue_mask_size);
  log_info("MBoot Bootdev", "framebuffer_green_field_position=%x", framebuffer->framebuffer_green_field_position);
  log_info("MBoot Bootdev", "framebuffer_green_mask_size=%x", framebuffer->framebuffer_green_mask_size);
  log_info("MBoot Bootdev", "framebuffer_red_field_position=%x", framebuffer->framebuffer_red_field_position);
  log_info("MBoot Bootdev", "framebuffer_red_mask_size=%x", framebuffer->framebuffer_red_mask_size);
  log_info("MBoot Bootdev", "framebuffer_palette=%x", framebuffer->framebuffer_palette);
  log_info("MBoot Bootdev", "framebuffer_palette_num_colors=%x", framebuffer->framebuffer_palette_num_colors);
}

static void mboot2_helper_print_tag_elf_sections(struct multiboot_tag_elf_sections* elf_sections) {
  log_info("MBoot Elf Sections", "entsize   =%x", elf_sections->entsize);
  log_info("MBoot Elf Sections", "num       =%x", elf_sections->num);
  log_info("MBoot Elf Sections", "shndx     =%x", elf_sections->shndx);
}

static void mboot2_helper_print_tag_load_base_addr(struct multiboot_tag_load_base_addr* load_base_addr) {
  log_info("MBoot Load Base Addr", "load_base_addr =%x", load_base_addr->load_base_addr);
}

void mboot2_helper_print_tag(struct multiboot_tag* tag) {
  log_group_begin("mboot2 tag");
  switch (tag->type) {
    case MULTIBOOT_TAG_TYPE_MMAP: 
      mboot2_helper_print_tag_mmap((struct multiboot_tag_mmap*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_APM: 
      mboot2_helper_print_tag_apm((struct multiboot_tag_apm*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
      mboot2_helper_print_tag_bootloader_name((struct multiboot_tag_string*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_CMDLINE:
      mboot2_helper_print_tag_cmdline((struct multiboot_tag_string*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_ELF_SECTIONS:
      mboot2_helper_print_tag_elf_sections((struct multiboot_tag_elf_sections*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_ACPI_NEW:
      mboot2_helper_print_tag_acpi_new((struct multiboot_tag_new_acpi*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_ACPI_OLD:
      mboot2_helper_print_tag_acpi_old((struct multiboot_tag_old_acpi*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
      mboot2_helper_print_tag_basic_meminfo((struct multiboot_tag_basic_meminfo*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_BOOTDEV:
      mboot2_helper_print_tag_bootdev((struct multiboot_tag_bootdev*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
      mboot2_helper_print_tag_framebuffer((struct multiboot_tag_framebuffer*)tag);
      break;
    case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
      mboot2_helper_print_tag_load_base_addr((struct multiboot_tag_load_base_addr*)tag);
      break;
    default:
      log_info("MBoot Tag Unknown", "type=%d", tag->type);
  }
  log_group_end("mboot2 tag");
}

void mboot2_print(void* mb_info_ptr) {
   uint8_t* ptr = (uint8_t*) mb_info_ptr;
   uint32_t size = *(uint32_t*)ptr;
   uint32_t reserved = *(uint32_t*)(ptr + 4);
   log_info("Multiboot Info", "size: %u", size);
   log_info("Multiboot Info", "reserved: %u", reserved);

   struct multiboot_tag *tag = (struct multiboot_tag *) (ptr + 8);
   while (tag->type != 0) {
     mboot2_helper_print_tag(tag);
     tag = (struct multiboot_tag*) ((uint8_t*)tag + ((tag->size + 7) & ~7));
   }
}
