# $@ = target file
# $< = first dependency
# $^ = all dependencies

# First rule is the one executed when no parameters are fed to the Makefile


# KERNEL_C_SRCS=$(wildcard src/kernel/*.c) $(wildcard src/kernel/**/*.c)
KERNEL_CPP_SRCS=$(wildcard src/kernel/*.cpp) $(wildcard src/kernel/**/*.cpp) $(wildcard src/lib/*.cpp)
KERNEL_ASM_SRCS=$(wildcard src/kernel/*.asm)
KERNEL_OBJS= $(KERNEL_ASM_SRCS:.asm=.o) $(KERNEL_CPP_SRCS:.cpp=.o)

all: clean dir run clean_after

dir:
	mkdir build

%.o: %.cpp
	i686-elf-gcc -m32 -xc++ -ffreestanding -fno-rtti -fno-exceptions -xnone -c $< -o $@ -fno-pie -fpermissive -Isrc/include

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
	qemu-system-i386 -drive format=raw,file=$<,index=0,if=floppy -boot a -hda disk.img -monitor stdio
clean:
	$(RM) -r build
	$(RM) -f ./**/**/*.o
	$(RM) -f ./**/**/**/*.o

clean_after:
	$(RM) -f ./**/**/*.o
	$(RM) -f ./**/**/**/*.o