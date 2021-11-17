# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile


KERNEL_C_SRCS=$(wildcard src/kernel/*.c)
KERNEL_ASM_SRCS=$(wildcard src/kernel/*.asm)
KERNEL_OBJS=$(KERNEL_C_SRCS:.c=.o) $(KERNEL_ASM_SRCS:.asm=.o)

all: clean dir run

dir:
	mkdir build

%.o: %.c
	i686-elf-gcc -m32 -ffreestanding -c $< -o $@ -fno-pie -Isrc/include

%.o: %.asm
	nasm $< -f elf -o $@


build/kernel.bin: build/kernel-entry.o $(KERNEL_OBJS) 
	i686-elf-ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary


# build/kernel.o: src/kernel/kernel.c
# 	i686-elf-gcc -m32 -ffreestanding -c src/kernel/kernel.c -o build/kernel.o -fno-pie

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
	$(RM) -f ./**/**/*.o