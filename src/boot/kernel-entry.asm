; this file will enable pagination to set up a higer half kernel mapped to 0xC0000000
[bits 32]


DIRECTORY_ADDRESS equ 0x100000
TABLE_ADDRESS_0 equ 0x101000
TABLE_ADDRESS_768 equ 0x300000
TABLE_ADDRESS_769 equ 0x301000

; clear directory
mov eax, DIRECTORY_ADDRESS
loop_clear_directory:
mov long [eax], 0
add eax, 4
cmp eax, DIRECTORY_ADDRESS + 4096
jl loop_clear_directory

; map table 0 to 0x00000000 ==> 0x00000000
mov eax, TABLE_ADDRESS_0
mov ebx, 7
loop_table_0:
mov long [eax], ebx
add eax, 4
add ebx, 4096
cmp eax, TABLE_ADDRESS_0 + 4096
jl loop_table_0

; map table 768 to 0xC0000000 ==> 0x00000000
mov eax, TABLE_ADDRESS_768
mov ebx, 7
loop_table_768:
mov long [eax], ebx
add eax, 4
add ebx, 4096
cmp eax, TABLE_ADDRESS_768 + 4096
jl loop_table_768


; map table 769 to 0xC0400000 ==> 0x00400000
mov eax, TABLE_ADDRESS_769
mov ebx, 0x400007
loop_table_769:
mov long [eax], ebx
add eax, 4
add ebx, 4096
cmp eax, TABLE_ADDRESS_769 + 4096
jl loop_table_769


mov long [DIRECTORY_ADDRESS], TABLE_ADDRESS_0  ; put tables addresses into directory
or long [DIRECTORY_ADDRESS], 7
mov long [DIRECTORY_ADDRESS + 768 * 4], TABLE_ADDRESS_768
or long [DIRECTORY_ADDRESS + 768 * 4], 7
mov long [DIRECTORY_ADDRESS + 769 * 4], TABLE_ADDRESS_769
or long [DIRECTORY_ADDRESS + 769 * 4], 7


mov eax, DIRECTORY_ADDRESS ; active pagination
mov cr3, eax
mov eax, cr0
or eax, 0x80000000
mov cr0, eax



jmp (up + 0xC0000000)

up:

pop long eax    ; remap gdt
mov ecx, eax
add ecx, 2
mov ebx, [ecx]
add ebx, 0xC0000000
mov [ecx], ebx
lgdt [eax]

mov ebp, 0xC0090000        ; setup stack
mov esp, ebp

push long ebx ; push gdt address 
; xchg bx, bx

mov long [DIRECTORY_ADDRESS], 0
mov ebx, cr3
mov cr3, ebx

[extern main]
call main
jmp $ ; infinite loop