#include "gdt.h"
#include "log.h"

extern void gdt_flush();
extern void tss_flush();

#define GDT_SIZE 6
struct gdt_entry gdt_entries[GDT_SIZE];
struct gdt_ptr gdt_ptr;
struct tss_entry tss_entry_inst;

void gdt_init(){
    log_info("GDT Init   ", "Begin\n");
    gdt_ptr.base = (uint32_t)&gdt_entries;
    gdt_ptr.limit = (sizeof (struct gdt_entry) * GDT_SIZE) - 1;

    // see https://wiki.osdev.org/Global_Descriptor_Table for the access explanation
    gdt_set_gate(0, 0, 0, 0, 0);                // null segment
    gdt_set_gate(1, 0, 0xffffffff, 0x9A, 0xCF); // kernel code segment
    gdt_set_gate(2, 0, 0xffffffff, 0x92, 0xCF); // kernel data segment
    gdt_set_gate(3, 0, 0xffffffff, 0xFA, 0xCF); // user   code segment
    gdt_set_gate(4, 0, 0xffffffff, 0xF2, 0xCF); // user   data segment
    write_tss   (5, 0x10, 0x0);                 // task  state segment 
    gdt_flush();
    log_info("GDT Init   ", "Flushed gdt\n");
    tss_flush();
    log_info("GDT Init   ", "Flushed tss\n");
    log_info("GDT Init   ", "Finished\n");
}

void gdt_set_gate(uint32_t n, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
    struct gdt_entry* e = &gdt_entries[n];
    e->baselo  = base & 0xffff;
    e->basemid = (base >> 16) & 0xff;
    e->basehi  = (base >> 24) & 0xff;

    e->limit = limit & 0xffff;
    e->flags_limit = (limit >> 16) & 0x0f;
    e->flags_limit |= (flags & 0xf0);

    e->access = access;
    log_info("GDT SetGate", "#%d    base=%x    limit=%x\n", n, base, limit);
}

void write_tss(uint32_t n, uint16_t ss0, uint32_t esp0) {
    if (n >= GDT_SIZE) {
        log_err("GDT WriteTss", "Can't write tss to full gdt (size: %d)", GDT_SIZE);
        return;
    }
    uint32_t base = (uint32_t) &tss_entry_inst;
    uint32_t limit = base + sizeof(struct tss_entry);

    // E9 -> 1110 1001 -> 
    //  1   Present,
    //  11  User priveledge level (3),
    //  0   system segment,
    //  1   Executable
    //  0   <not applicable>
    //  0   <not applicable>
    //  1   Accessed
    uint8_t access  = 0b11101001;
    gdt_set_gate(n, base, limit, access, 0x00);

    // initialize tss
    memset(&tss_entry_inst, 0, sizeof(struct tss_entry));

    tss_entry_inst.ss0 = ss0;       // kernel stack gdt segment
    tss_entry_inst.esp0 = esp0;
    // 0x3 = 0011 (see TSS osdev)
    tss_entry_inst.cs = 0x08 | 0x3; //allows switching from user to kernel mode
    tss_entry_inst.ss = 0x10 | 0x3;
    tss_entry_inst.ds = 0x10 | 0x3;
    tss_entry_inst.es = 0x10 | 0x3;
    tss_entry_inst.fs = 0x10 | 0x3;
    tss_entry_inst.gs = 0x10 | 0x3;
    log_info("GDT WriteTSS", "cs = %x, ss = %x, ds = %x\n", tss_entry_inst.cs, tss_entry_inst.ss, tss_entry_inst.ds);
    log_info("GDT WriteTSS", "fs = %x, es = %x, gs = %x\n", tss_entry_inst.fs, tss_entry_inst.es, tss_entry_inst.gs);
}
