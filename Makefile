# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile

all: clean dir run

dir:
	mkdir build


build/kernel.bin: build/kernel-entry.o build/kernel.o build/isr.o
	i686-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary


build/kernel.o: src/kernel/kernel.c
	i686-elf-gcc -m32 -ffreestanding -c src/kernel/kernel.c -o build/kernel.o -fno-pie

build/kernel-entry.o: src/boot/kernel-entry.asm
	nasm src/boot/kernel-entry.asm -f elf -o build/kernel-entry.o

build/isr.o: src/kernel/isr.asm
	nasm src/kernel/isr.asm -felf -o build/isr.o

build/boot.bin: src/boot/boot.asm
	nasm src/boot/boot.asm -f bin -o build/boot.bin -i src/boot

build/os-image.bin: build/boot.bin build/kernel.bin
	cat $^ > $@

run: build/os-image.bin
	qemu-system-i386 -fda $<

clean:
	$(RM) -r build