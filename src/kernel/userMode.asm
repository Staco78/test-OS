global jump_userMode

extern user_func
jump_userMode:
    cli
    mov ax, (4 * 8) | 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; xor edx, edx
    ; mov eax, 0x100008
    ; mov ecx, 0x174
    ; wrmsr

    ; mov edx, user_func
    ; mov ecx, esp
    ; sysexit

    ; xchg bx, bx

    mov eax, esp
    push (4 * 8) | 3
    push eax
    pushf
    pop eax
    or eax, 0x200
    push eax
    push (3 * 8) | 3
    push user_func
    iret