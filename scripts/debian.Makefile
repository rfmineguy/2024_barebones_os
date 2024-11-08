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
OUT := out

C_KERNEL_SOURCE := $(wildcard $(KERNEL_SRC)/*.c)
C_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/*.c)

S_KERNEL_SOURCE := $(wildcard $(KERNEL_SRC)/*.s)
S_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/*.s)

C_SOURCES := $(C_KERNEL_SOURCE) $(C_STDLIB_SOURCE)
S_SOURCES := $(S_KERNEL_SOURCE) $(S_STDLIB_SOURCE)

C_OBJECTS := $(patsubst $(KERNEL_SRC)/%.c, $(OUT)/%.c.o, $(C_KERNEL_SOURCE)) $(patsubst $(STDLIB_SRC)/%.c, $(OUT)/%.c.o, $(C_STDLIB_SOURCE))
S_OBJECTS := $(patsubst $(KERNEL_SRC)/%.s, $(OUT)/%.s.o, $(S_KERNEL_SOURCE)) $(patsubst $(STDLIB_SRC)/%.s, $(OUT)/%.s.o, $(S_STDLIB_SOURCE))

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
always: create_fat_fs
	mkdir -p $(OUT)

clean:
	-rm -r $(OUT)/

# Build related targets
build: always $(OUT)/$(BIN) grub_gen_rescue

$(OUT)/%.c.o: $(KERNEL_SRC)/%.c
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_SRC) -I$(STDLIB_SRC) -I$(OUT)/

$(OUT)/%.c.o: $(STDLIB_SRC)/%.c
	$(CC) $(GCCFLAGS) -c $^ -o $@ $(CFLAGS) $(OPTIMIZATION_FLAGS) -Wall -Wextra -I$(KERNEL_SRC) -I$(STDLIB_SRC) -I$(OUT)/

$(OUT)/%.s.o: $(KERNEL_SRC)/%.s
	$(AS) $(ASFLAGS) $^ -o $@

$(OUT)/%.s.o: $(STDLIB_SRC)/%.s
	$(AS) $(ASFLAGS) $^ -o $@

$(OUT)/$(BIN): $(C_OBJECTS) $(S_OBJECTS)
	$(LD) -T linker.ld -o $@ -nostdlib $(S_OBJECTS) $(C_OBJECTS) -L$(LIBDIR) -lgcc

# Grub related targets
#
# set timeout=5 
# set default=0 
#  
# menuentry "myos\" {
#     insmod fat 
#     set root=(cd) 
#     # fatload ${root} /main.img 0x80000000 
#     multiboot /boot/os.bin 
#     boot 
# } 
grub_gen_cfg_fs:
	# echo "    fatload ${root} /main.img 0x80000000";
	# echo "    cat /main.img 0x01000000";
	@{ \
		echo "set timeout=5"; \
		echo "set default=0"; \
		echo ""; \
		echo "menuentry \"myos\" {"; \
		echo "    insmod fat"; \
		echo "    insmod part_msdos"; \
		echo "    multiboot /boot/os.bin"; \
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
		echo "    multiboot /boot/os.bin";\
		echo "    boot";\
		echo "}"; \
	} > grub.cfg

grub_gen_rescue: grub_gen_cfg_fs
	mkdir -p isodir/boot/grub
	dd if=out/os.bin of=isodir/boot/os.bin
	dd if=grub.cfg of=isodir/boot/grub/grub.cfg
	dd if=out/main.img of=isodir/main.img
	grub-mkrescue -o out/os.iso isodir
	#rm grub.cfg
	#rm -r isodir

grub_check_multiboot:
	@echo "Checking multiboot"
	@if grub-file --is-x86-multiboot out/os.bin; then \
		echo "Multiboot confirmed"; \
	else \
		echo "Multiboot not present "; \
	fi

# File system targets
create_fat_fs:
	dd if=/dev/zero of=out/main.img bs=512 count=2880
	mkfs.fat -F 12 -n "RFOS" out/main.img
	mcopy -i out/main.img fatfiles/test.txt "::test.txt"
	mcopy -i out/main.img fatfiles/a.txt "::a.txt"
