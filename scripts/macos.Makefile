SRC := src
OUT := out

C_SOURCES := $(wildcard $(SRC)/*.c)
S_SOURCES := $(wildcard $(SRC)/*.s)

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
	i686-elf-gcc -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

$(OUT)/%.s.o: $(SRC)/%.s
	i686-elf-as $^ -o $@

$(OUT)/$(BIN): $(C_OBJECTS) $(S_OBJECTS)
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(S_OBJECTS) $(C_OBJECTS) -lgcc
