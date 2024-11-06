#include "multiboot.h"
#include "log.h"

void multiboot_debug_raw(struct multiboot_info* info) {
    for (uint32_t i = 0; i < sizeof(struct multiboot_info) / sizeof(uint32_t); i++) {
        log_info("MBootRaw", "0x%X: %u\n", info + i, *(info + i));
    }
}

void multiboot_debug(struct multiboot_info* info) {
  log_info("Multiboot", "flags: %b"              "\n"   , info->flags); 
  log_info("Multiboot", "mem_lower: %d"          "\n"   , info->mem_lower);
  log_info("Multiboot", "mem_upper: %d"          "\n"   , info->mem_upper);
  log_info("Multiboot", "boot_device: %d"        "\n"   , info->boot_device);
  log_info("Multiboot", "cmdline: %d"            "\n"   , info->cmdline);
  log_info("Multiboot", "mods_count: %d"         "\n"   , info->mods_count);
  log_info("Multiboot", "mods_addr: %d"          "\n"   , info->mods_addr);
  log_info("Multiboot", "mmap_length: %d"        "\n"   , info->mmap_length);
  log_info("Multiboot", "mmap_addr: %d"          "\n"   , info->mmap_addr);
  log_info("Multiboot", "drives_length: %d"      "\n"   , info->drives_length);
  log_info("Multiboot", "drives_addr: %d"        "\n"   , info->drives_addr);
  log_info("Multiboot", "config_table: %d"       "\n"   , info->config_table);
  log_info("Multiboot", "boot_loader_name: %d"   "\n"   , info->boot_loader_name);
  log_info("Multiboot", "apm_table: %d"          "\n"   , info->apm_table);
  log_info("Multiboot", "vbe_control_info: %d"   "\n"   , info->vbe_control_info);
  log_info("Multiboot", "vbe_mode_info: %d"      "\n"   , info->vbe_mode_info);
  log_info("Multiboot", "vbe_mode: %d"           "\n"   , info->vbe_mode);
  log_info("Multiboot", "vbe_interface_seg: %d"  "\n"   , info->vbe_interface_seg);
  log_info("Multiboot", "vbe_interface_off: %d"  "\n"   , info->vbe_interface_off);
  log_info("Multiboot", "vbe_interface_len: %d"  "\n"   , info->vbe_interface_len);
}

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#kernel_002ec
void multiboot_verify(int magic, struct multiboot_info* mbi) {

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        log_info ("MbootVerify", "Invalid magic number: 0x%x\n", (unsigned) magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    /* Print out the flags. */
    log_info ("MbootVerify", "flags = %b\n", (unsigned) mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG (mbi->flags, 0)) {
        log_info("MbootVerify", "MemFlags\n");
        log_info("MbootVerify", "\tmem_lower = %uKB, mem_upper = %uKB\n",
            (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);
    }

    /* Is boot_device valid? */
    if (CHECK_FLAG (mbi->flags, 1)) {
        log_info("MbootVerify", "Boot Device\n");
        log_info("MbootVerify", "\tboot_device = 0x%x\n", (unsigned) mbi->boot_device);
    }

    /* Is the command line passed? */
    if (CHECK_FLAG (mbi->flags, 2)) {
        log_info("MbootVerify", "Flags\n");
        log_info("MbootVerify", "\tcmdline = %s\n", (char *) mbi->cmdline);
    }

  /* Are mods_* valid? */
  if (CHECK_FLAG (mbi->flags, 3)) {
      struct multiboot_module_s *mod;
      int i;

      log_info ("MbootVerify", "mods_count = %d, mods_addr = 0x%x\n", (int) mbi->mods_count, (int) mbi->mods_addr);
      for (i = 0, mod = (struct multiboot_module_s *) mbi->mods_addr; i < mbi->mods_count; i++, mod++)
        log_info ("MbootVerify", "mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
                (unsigned) mod->mod_start,
                (unsigned) mod->mod_end,
                (char *) mod->string);
    }
}
