section .text
global _start
_start:
    ; xchg bx, bx
    mov eax, 2
x:
    int 0x80
    int 0x80
    int 0x80
    int 0x80
    jmp x