%macro IRQ 1
global _irq_%1
_irq_%1:
cli
pusha
push ds
push es
push fs
push gs

mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
extern irq_handler_%1
call irq_handler_%1

mov eax, 0x20
%if %1 >= 8
    mov edx, 0xA0 
    out dx, al
%endif
mov edx, 0x20
out dx, al

pop gs
pop fs
pop es
pop ds
popa
sti 
iret
%endmacro

IRQ 1
IRQ 8