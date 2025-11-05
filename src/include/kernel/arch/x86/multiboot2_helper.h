#ifndef MULTIBOOT2_HELPER_H
#define MULTIBOOT2_HELPER_H
#include "multiboot2.h"

void mboot2_helper_print_tag(struct multiboot_tag*);

// pass the info ptr from kmain
void mboot2_print(void* mb_info_ptr);

#endif
