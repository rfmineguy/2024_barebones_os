#ifndef MULTIBOOT_H
#define MULTIBOOT_H
#include <stdint.h>
// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

/* The symbol table for a.out. */
struct multiboot_aout_symbol_table
{
  uint32_t tabsize;
  uint32_t strsize;
  uint32_t addr;
  uint32_t reserved;
};

/* The section header table for ELF. */
struct multiboot_elf_section_header_table
{
  uint32_t num;
  uint32_t size;
  uint32_t addr;
  uint32_t shndx;
};

struct multiboot_info
{
  /* Multiboot info version number */
  uint32_t flags;

  /* Available memory from BIOS */
  uint32_t mem_lower;
  uint32_t mem_upper;

  /* "root" partition */
  uint32_t boot_device;

  /* Kernel command line */
  uint32_t cmdline;

  /* Boot-Module list */
  uint32_t mods_count;
  uint32_t mods_addr;

  union
  {
    struct multiboot_aout_symbol_table aout_sym;
    struct multiboot_elf_section_header_table elf_sec;
  } u;

  /* Memory Mapping buffer */
  uint32_t mmap_length;
  uint32_t mmap_addr;

  /* Drive Info buffer */
  uint32_t drives_length;
  uint32_t drives_addr;

  /* ROM configuration table */
  uint32_t config_table;

  /* Boot Loader Name */
  uint32_t boot_loader_name;

  /* APM table */
  uint32_t apm_table;

  /* Video */
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;

  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t framebuffer_bpp;
#define FRAMEBUFFER_TYPE_INDEXED 0
#define FRAMEBUFFER_TYPE_RGB     1
#define FRAMEBUFFER_TYPE_EGA_TEXT     2
  uint8_t framebuffer_type;
  union
  {
    struct
    {
      uint32_t framebuffer_palette_addr;
      uint16_t framebuffer_palette_num_colors;
    };
    struct
    {
      uint8_t framebuffer_red_field_position;
      uint8_t framebuffer_red_mask_size;
      uint8_t framebuffer_green_field_position;
      uint8_t framebuffer_green_mask_size;
      uint8_t framebuffer_blue_field_position;
      uint8_t framebuffer_blue_mask_size;
    };
  };
};

struct multiboot_mmap_entry
{
  uint32_t size;
  uint32_t addrlo;
  uint32_t addrhi;
  uint32_t lenlo;
  uint32_t lenhi;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
  uint32_t type;
} __attribute__((packed));

struct module_s {
    uint32_t mod_start;   // The starting address of the module in memory
    uint32_t mod_end;     // The ending address of the module in memory
    uint32_t string;      // The command line string associated with the module
    uint32_t reserved;    // Reserved for future use
} __attribute__((packed));

#endif
