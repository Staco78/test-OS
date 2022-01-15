extern syscall_c
global syscall
syscall:
    pusha
    call syscall_c
    popa
    iret

    ; xchg bx, bx