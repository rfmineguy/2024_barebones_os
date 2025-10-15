#include "io/log.h"

void kernel_main(int magic, struct multiboot_header* header) {
     serial_init();
     log_info("test", "ad");

     for(;;);
}
