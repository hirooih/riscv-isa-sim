CC = riscv64-unknown-elf-gcc
build:
	$(CC) -g -Og -o rot13-64.o -c rot13.c
	$(CC) -g -Og -T spike.lds -nostartfiles -o rot13-64 rot13-64.o

spike:
	spike --rbb-port=9824 -m0x10100000:0x20000 rot13-64

openocd:
	openocd -f spike.cfg

gdb:
	riscv64-unknown-elf-gdb rot13-64

clean:
	rm -f rot13-64.o rot13-64
