KERNEL_SRC := src/kernel
STDLIB_SRC := src/stdlib
OUT := out

C_KERNEL_SOURCE := $(wildcard $(KERNEL_SRC)/*.c)
C_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/*.c)

S_KERNEL_SOURCE := $(wildcard $(KERNEL_SRC)/*.s)
S_STDLIB_SOURCE := $(wildcard $(STDLIB_SRC)/*.s)

C_SOURCES := $(C_KERNEL_SOURCE) $(C_STDLIB_SOURCE)
S_SOURCES := $(S_KERNEL_SOURCE) $(S_STDLIB_SOURCE)

C_OBJECTS := $(patsubst $(SRC)/%.c, $(OUT)/%.c.o, $(C_SOURCES))
S_OBJECTS := $(patsubst $(SRC)/%.s, $(OUT)/%.s.o, $(S_SOURCES))

BIN := os.bin

.PHONY: always clean build
always:
	mkdir -p $(OUT)

clean:
	rm -r $(OUT)/

build: always $(OUT)/$(BIN)

$(OUT)/%.c.o: $(SRC)/%.c
	i686-elf-gcc -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I$(KERNEL_SRC) -I$(STDLIB_SRC)

$(OUT)/%.s.o: $(SRC)/%.s
	i686-elf-as $^ -o $@

$(OUT)/$(BIN): $(C_OBJECTS) $(S_OBJECTS)
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(S_OBJECTS) $(C_OBJECTS) -lgcc -I$(KERNEL_SRC) -I$(STDLIB_SRC)

