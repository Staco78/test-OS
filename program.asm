section .data
    msg db "Hello world", 10
    len equ $ - msg

section .text
global _start
_start:
    mov eax, 0
    mov ebx, msg
    mov ecx, len
    int 0x80

    mov eax, 1
    int 0x80