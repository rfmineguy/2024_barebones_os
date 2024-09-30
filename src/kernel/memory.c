#include "memory.h"
#include "printf.h"
#include "log.h"

void memory_init(struct multiboot_info* info) {
    for (int i = 0; i < info->mmap_length; i += sizeof(struct multiboot_mmap_entry)) {
        struct multiboot_mmap_entry *mmmt = (struct multiboot_mmap_entry*)(info->mmap_addr+i);

        log_info("MemInit ", "loaddr: %x, hiaddr: %x, lolen: %x, hilen: %x, size: %x, type: %d\n",
                mmmt->addrlo, mmmt->addrhi, mmmt->lenlo, mmmt->lenhi, mmmt->size, mmmt->type);
    }
}
