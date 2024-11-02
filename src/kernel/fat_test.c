#include "fat_test.h"
#include "../stdlib/printf.h"
#include "../stdlib/ctype.h"
#include "serial.h"
#include "log.h"

void fat_test(uint32_t start) {
    log_info("FatTest", "Begin\n");
    char* fat = (char*) start;
    for (int i = 0; i < 0xc0; i+=1) {
        if (isprint((char)fat[i])) {
            serial_printf("%c", (char)fat[i]);
        }
        else {
            serial_printf(".");
        }
    }
    serial_printf("\n");
    log_info("FatTest", "End\n");
    // k_printf("fat_test complete. see serial output...\n");
}
