nasm -felf32 program.asm -o program.o
ld -melf_i386 -o program program.o
rm program.o