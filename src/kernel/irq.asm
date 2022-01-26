%macro IRQ 1
global _irq_%1
_irq_%1:
cli
; push byte 0x00
; push byte 0x00
; xchg bx, bx
pusha
; and esp, 0xFFFFFFF0
; sub esp, 16
; xchg bx, bx
; xchg bx, bx

; mov ax, ds
; push eax

mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

; push 0x12345678


extern irq_handler_%1
call irq_handler_%1

; sub esp, 4
; pop eax
; add esp, 0x14

mov eax, 0x20
%if %1 >= 8
    mov edx, 0xA0 
    out dx, al
%endif
mov edx, 0x20
out dx, al

mov ax, (4 * 8) | 3
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax

; pop ebx
; mov ds, bx
; mov es, bx
; mov fs, bx
; mov gs, bx
; xchg bx, bx
; add esp, 16
popa
; xchg bx, bx

sti 
nop
iret
%endmacro

%macro ISR 1
global _isr_%1
_isr_%1:
cli
pusha

mov ax, 0x10
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
extern %1
call %1

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

IRQ 1
IRQ 8

ISR syscall