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
TEST_SRC   := src/tests
OUT := out

C_KERNEL_SOURCE := $(wildcard $(KERNEL_SRC)/*.c)
C_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/*.c)

C_TEST_SOURCE   := $(wildcard $(TEST_SRC)/*.c)

S_KERNEL_SOURCE := $(wildcard $(KERNEL_SRC)/*.s)
S_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/*.s)

C_SOURCES := $(C_KERNEL_SOURCE) $(C_STDLIB_SOURCE)
S_SOURCES := $(S_KERNEL_SOURCE) $(S_STDLIB_SOURCE)

C_OBJECTS := $(patsubst $(KERNEL_SRC)/%.c, $(OUT)/%.c.o, $(C_KERNEL_SOURCE)) \
			 $(patsubst $(STDLIB_SRC)/%.c, $(OUT)/%.c.o, $(C_STDLIB_SOURCE)) \
			 $(patsubst $(TEST_SRC)/%.c, $(OUT)/%.c.o, $(C_TEST_SOURCE))

S_OBJECTS := $(patsubst $(KERNEL_SRC)/%.s, $(OUT)/%.s.o, $(S_KERNEL_SOURCE)) \
			 $(patsubst $(STDLIB_SRC)/%.s, $(OUT)/%.s.o, $(S_STDLIB_SOURCE))

BIN := os.bin
OPTIMIZATION_FLAGS := -O0
CFLAGS := -std=gnu99 -ffreestanding -nostdlib -m32 -ggdb
ASFLAGS := -g
GCCFLAGS := -B/home/build

SHELL := /bin/bash
CC := /home/build/gcc
AS := /home/build/as
LD := /home/build/ld
LIBDIR := /home/build/lib/gcc/i686-elf/7.1.0/

.PHONY: grub_gen_cfg grub_gen_rescue grub_check_multiboot
.PHONY: create_fat_fs
.PHONY: always clean build
# create_fat_fs
always:
	mkdir -p $(OUT)

clean:
	-rm -r $(OUT)/

# Build related targets
build: always $(OUT)/$(BIN) grub_gen_rescue

$(OUT)/%.c.o: $(KERNEL_SRC)/%.c
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_SRC) -I$(STDLIB_SRC) -I$(TEST_SRC) -I$(OUT)/

$(OUT)/%.c.o: $(STDLIB_SRC)/%.c
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_SRC) -I$(STDLIB_SRC) -I$(TEST_SRC) -I$(OUT)/

$(OUT)/%.c.o: $(TEST_SRC)/%.c
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_SRC) -I$(STDLIB_SRC) -I$(TEST_SRC) -I$(OUT)/

$(OUT)/%.s.o: $(KERNEL_SRC)/%.s
	$(AS) $(ASFLAGS) $^ -o $@

$(OUT)/%.s.o: $(STDLIB_SRC)/%.s
	$(AS) $(ASFLAGS) $^ -o $@

$(OUT)/$(BIN): $(C_OBJECTS) $(S_OBJECTS)
	$(LD) -T linker.ld -o $@ -nostdlib $(S_OBJECTS) $(C_OBJECTS) -L$(LIBDIR) -lgcc

# Grub related targets
grub_gen_cfg_fs:
	@{ \
		echo "set timeout=5"; \
		echo "set default=0"; \
		echo ""; \
		echo "menuentry \"myos\" {"; \
		echo "    insmod fat"; \
		echo "    insmod part_msdos"; \
		echo "    multiboot2 /boot/os.bin"; \
		echo "    module /main.img \"fat_image\""; \
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
	dd if=out/main.img of=isodir/main.img
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
	dd if=/dev/zero of=out/main.img bs=512 count=2880
	mkfs.fat -F 12 -n "RFOS" out/main.img
	mcopy -i out/main.img fatfiles/test.txt "::test.txt"
	mcopy -i out/main.img fatfiles/a.txt "::a.txt"

create_qcow:
	qemu-img create -f qcow2 out/main.qcow2 50M
	mkfs.fat -F 12 -n "RFOS" out/main.qcow2
	mcopy -i out/main.qcow2 fatfiles/test.txt "::test.txt"
	mcopy -i out/main.qcow2 fatfiles/a.txt "::a.txt"
	qemu-img info out/main.qcow2
