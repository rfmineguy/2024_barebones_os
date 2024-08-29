C_SOURCES := $(wildcard src/*.c)
S_SOURCES := $(wildcard src/*.s)

C_OBJECTS := $(patsubst src/%.c, out/%.c.o, $(C_SOURCES))
S_OBJECTS := $(patsubst src/%.s, out/%.s.o, $(S_SOURCES))

BIN := os.bin

.PHONY: always clean build
always:
	mkdir -p out/

clean:
	rm -r out/

build: always out/$(BIN)

out/%.c.o: src/%.c
	i686-elf-gcc -c $^ -o $@ -std=gnu99 -ffreestanding -O2 -Wall -Wextra

out/%.s.o: src/%.s
	i686-elf-as $^ -o $@

out/$(BIN): $(C_OBJECTS) $(S_OBJECTS)
	i686-elf-gcc -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(S_OBJECTS) $(C_OBJECTS) -lgcc
