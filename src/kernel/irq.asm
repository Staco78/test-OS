; %1: name, %2: func to call, %3 irq number (-1 if not)
%macro interrupt 3
global %1
%1:
cli
pusha

mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

push esp

cld

extern %2
call %2

add esp, 4

%if %3 >= 0
    mov eax, 0x20
    %if %3 >= 8
        mov edx, 0xA0 
        out dx, al
    %endif
    mov edx, 0x20
    out dx, al
%endif

mov ax, (4 * 8) | 3
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
popa
sti
nop
iret
%endmacro

%macro IRQ 1
interrupt _irq_%1, irq_handler_%1, %1
%endmacro

%macro ISR 1
interrupt _isr_%1, %1, -1
%endmacro

IRQ 1
IRQ 8

ISR syscall