# ================================================================================
# How to use this makefile
#    This makefile expects a specific environment to exist
#    The environment can be got from the alpine dockerfile in the docker/ directory
#
# docker run --rm -it -w /root/workspace -v $(pwd):/root/workspace alpine2-test
# make build -f scripts/alpine.Makefile
# ================================================================================

KERNEL_SRC := src/kernel
STDLIB_SRC := src/stdlib
TPL_SRC 	 := src/tpl

KERNEL_INC := src/include/kernel
STDLIB_INC := src/include/stdlib

KERNEL_DATASTRUCTURE_SRC     := $(KERNEL_SRC)/datastructures
KERNEL_DATASTRUCTURE_INCLUDE := $(KERNEL_INC)/datastructures

KERNEL_MAIN_ENTRY := $(KERNEL_SRC)/entrypoints/kernel.c
KERNEL_TEST_ENTRY := $(KERNEL_SRC)/entrypoints/kernel_test.c

TEST_SRC   := src/tests
OUT        := out

# ===================================
#  Generics generator variables
# ===================================
GENGEN_TPL_FILES := $(TPL_SRC)/queue.htpl \
										$(TPL_SRC)/queue.ctpl \
										$(TPL_SRC)/linkedlist.htpl \
										$(TPL_SRC)/linkedlist.ctpl

GENGEN_GEN_FILES := $(KERNEL_DATASTRUCTURE_SRC)/queue_event.c \
										$(KERNEL_DATASTRUCTURE_INCLUDE)/queue_event.h \
										$(KERNEL_DATASTRUCTURE_SRC)/linkedlist_memory_node.c \
										$(KERNEL_DATASTRUCTURE_INCLUDE)/linkedlist_memory_node.h

GENGEN_SOURCE_FILES := $(filter %.c,$(GENGEN_GEN_FILES))
GENGEN_OBJECTS := $(patsubst $(KERNEL_DATASTRUCTURE_SRC)/%.c, $(OUT)/datastructures/%.c.o, $(GENGEN_SOURCE_FILES))


# ===================================
#  Generics generator variables
# ===================================
C_KERNEL_BLACKLIST := $(KERNEL_MAIN_ENTRY) $(KERNEL_TEST_ENTRY) $(GENGEN_SOURCE_FILES)
#C_KERNEL_SOURCE := $(filter-out $(C_KERNEL_BLACKLIST), $(wildcard $(KERNEL_SRC)/**/*.c))
C_KERNEL_SOURCE := $(filter-out $(C_KERNEL_BLACKLIST), $(shell find $(KERNEL_SRC)/ -type f -name "*.c"))
C_STDLIB_SOURCE := $(shell find $(STDLIB_SRC)/ -type f -name "*.c")

C_TEST_SOURCE   := $(wildcard $(TEST_SRC)/*.c)

S_KERNEL_BLACKLIST := 
#S_KERNEL_SOURCE := $(filter-out $(S_KERNEL_BLACKLIST), $(wildcard $(KERNEL_SRC)/**/*.s))
S_KERNEL_SOURCE := $(filter-out $(S_KERNEL_BLACKLIST), $(shell find $(KERNEL_SRC)/ -type f -name "*.s"))
#S_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/**/*.s)
S_STDLIB_SOURCE := $(shell find $(STDLIB_SRC)/ -type f -name "*.s")

C_SOURCES := $(C_KERNEL_SOURCE) $(C_STDLIB_SOURCE)
S_SOURCES := $(S_KERNEL_SOURCE) $(S_STDLIB_SOURCE)

C_OBJECTS := $(patsubst $(KERNEL_SRC)/%.c, $(OUT)/%.c.o, $(C_KERNEL_SOURCE)) \
			 $(patsubst $(STDLIB_SRC)/%.c, $(OUT)/%.c.o, $(C_STDLIB_SOURCE))\
			 $(GENGEN_OBJECTS)
			 #$(patsubst $(TEST_SRC)/%.c, $(OUT)/%.c.o, $(C_TEST_SOURCE))

S_OBJECTS := $(patsubst $(KERNEL_SRC)/%.s, $(OUT)/%.s.o, $(S_KERNEL_SOURCE)) \
						 $(patsubst $(STDLIB_SRC)/%.s, $(OUT)/%.s.o, $(S_STDLIB_SOURCE))

C_LISTINGS:= $(patsubst $(OUT)/%.c.o, $(OUT)/%.c.o.lst, $(C_OBJECTS))
S_LISTINGS:= $(patsubst $(OUT)/%.s.o, $(OUT)/%.s.o.lst, $(S_OBJECTS))

ALL_OBJS := $(C_OBJECTS) $(S_OBJECTS)
OBJ_DIRS := $(patsubst %, $(OUT)/%, $(C_OBJECTS) $(S_OBJECTS))

BIN := os.bin
OPTIMIZATION_FLAGS := -O0
CFLAGS := -std=gnu99 -ffreestanding -nostdlib -m32 -ggdb -gdwarf
ASFLAGS := -g
GCCFLAGS := -B/home/build

SHELL := /bin/bash
CC := /home/build/gcc
AS := /home/build/as
LD := /home/build/ld
OBJDUMP := /home/build/bin/i686-elf-objdump
LIBDIR := /home/build/lib/gcc/i686-elf/7.1.0/

CC_BIG := gcc

.PHONY: grub_gen_cfg grub_gen_rescue grub_check_multiboot
.PHONY: create_fat_fs
.PHONY: always clean build
.PHONY: gen_lst
# create_fat_fs
always:
	mkdir -p $(OUT)
	@echo "C Object Files"
	@echo $(C_OBJECTS)
	@echo "C Object Files End"
	@echo "S Object Files"
	@echo $(S_OBJECTS)
	@echo "S Object Files End"

clean:
	-rm -r $(OUT)/
	-rm -r $(GENGEN_GEN_FILES)

# Build related targets
build: always $(GENGEN_GEN_FILES) $(OUT)/$(BIN) grub_gen_rescue gen_lst

$(OUT)/%.c.o: $(KERNEL_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_INC) -I$(STDLIB_INC) -I$(TEST_INC) -I$(OUT)/

$(OUT)/%.c.o: $(STDLIB_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_INC) -I$(STDLIB_INC) -I$(TEST_INC) -I$(OUT)/

$(OUT)/%.c.o: $(TEST_SRC)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_INC) -I$(STDLIB_INC) -I$(TEST_INC) -I$(OUT)/

$(OUT)/%.s.o: $(KERNEL_SRC)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $^ -o $@

$(OUT)/%.s.o: $(STDLIB_SRC)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $^ -o $@

$(OUT)/$(BIN): $(C_OBJECTS) $(S_OBJECTS)
	$(LD) -T linker.ld -o $@ -nostdlib $(S_OBJECTS) $(C_OBJECTS) -L$(LIBDIR) -lgcc

# Build gengen
$(OUT)/gengen: gengen.c
	$(CC_BIG) -o $@ $^

$(GENGEN_GEN_FILES): $(OUT)/gengen $(GENGEN_TPL_FILES)
	./$(OUT)/gengen

# Generate listing files
gen_lst: $(C_LISTINGS) $(S_LISTINGS)
$(OUT)/%.c.o.lst: $(OUT)/%.c.o
	$(OBJDUMP) -d -S $^ > $@
$(OUT)/%.s.o.lst: $(OUT)/%.s.o
	$(OBJDUMP) -d -S $^ > $@

# Grub related targets
grub_gen_cfg_fs:
	@{ \
		echo "set timeout=5"; \
		echo "set default=0"; \
		echo ""; \
		echo "menuentry \"myos\" {"; \
		echo "    multiboot2 /boot/os.bin"; \
		echo "    boot"; \
		echo "}"; \
	} > grub.cfg

grub_gen_cfg_nofs:
	@{ \
		echo "set timeout=5"; \
		echo "set default=0"; \
		echo ""; \
		echo ""; \
		echo "menuentry \"myos\" {"; \
		echo "    multiboot2 /boot/os.bin";\
		echo "    boot";\
		echo "}"; \
	} > grub.cfg

grub_gen_rescue: grub_gen_cfg_fs
	mkdir -p isodir/boot/grub
	dd if=out/os.bin of=isodir/boot/os.bin
	dd if=grub.cfg of=isodir/boot/grub/grub.cfg
	dd if=drives/main.img of=isodir/main.img
	grub-mkrescue -o out/os.iso isodir
	rm grub.cfg
	rm -r isodir

grub_check_multiboot: checkmboot1 checkmboot2
checkmboot1:
	@echo "Checking multiboot"
	@if grub-file --is-x86-multiboot out/os.bin; then \
		echo "Multiboot confirmed"; \
	else \
		echo "Multiboot not present "; \
	fi
checkmboot2:
	@echo "Checking multiboot2"
	@if grub-file --is-x86-multiboot2 out/os.bin; then \
		echo "Multiboot2 confirmed"; \
	else \
		echo "Multiboot2 not present "; \
	fi

# File system targets
create_fat_fs:
	mkdir -p drives
	dd if=/dev/zero of=drives/main.img bs=512 count=2880
	mkfs.fat -F 12 -n "RFOS" drives/main.img
	mcopy -i drives/main.img fatfiles/test.txt "::test.txt"
	mcopy -i drives/main.img fatfiles/a.txt "::a.txt"
	mcopy -i drives/main.img fatfiles/new.txt "::new.t"

create_qcow:
	mkdir -p drives
	qemu-img create -f qcow2 drives/main.qcow2 50M
	mkfs.fat -F 12 -n "RFOS" drives/main.qcow2
	mcopy -i drives/main.qcow2 fatfiles/test.txt "::test.txt"
	mcopy -i drives/main.qcow2 fatfiles/a.txt "::a.txt"
	mcopy -i drives/main.qcow2 fatfiles/new.txt "::new.txt"
	qemu-img info drives/main.qcow2
