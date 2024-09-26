KERNEL_SOURCES := ../src/stdlib/printf.c

.PHONY: always clean build
always:
	mkdir -p out
clean:
	rm -r out
build: always
	gcc -D TESTING src/*.c $(KERNEL_SOURCES) -o out/test -ggdb
