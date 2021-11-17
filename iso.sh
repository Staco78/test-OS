dd if=build/os-image.bin of=floppy.img seek=0 count=10 conv=notrunc
cp floppy.img iso/
genisoimage -V 'MYOS' -input-charset iso8859-1 -o myos.iso -b floppy.img iso/
qemu-system-i386 -cdrom myos.iso