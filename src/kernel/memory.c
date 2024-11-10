#include "memory.h"
#include "printf.h"
#include "log.h"

void memory_init(struct multiboot_info* info) {
    for (uint32_t i = 0; i < info->mmap_length; i += sizeof(struct multiboot_mmap_entry)) {
        struct multiboot_mmap_entry *mmmt = (struct multiboot_mmap_entry*)(info->mmap_addr+i);
        (void)mmmt;

    }
}
