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

.PHONY: always clean build
always:
	mkdir -p $(OUT)

clean:
	rm -r $(OUT)/

build: always $(OUT)/$(BIN)

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
