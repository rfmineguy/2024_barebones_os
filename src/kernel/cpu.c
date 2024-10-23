#include "cpu.h"

void get_registers() {
    uint32_t eax, ebx, ecx, edx;

    asm volatile (
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        : "=r"(eax), "=r"(ebx), "=r"(ecx), "=r"(edx)
    );
}
