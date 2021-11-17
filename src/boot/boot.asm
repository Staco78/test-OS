[bits 16]
[org 0x7c00]

; jmp 0000:start
; times 8-($-$$) db 0
; bit_PrimaryVolumeDescriptor   resd 1
; bit_BootFileLocation          resd 1
; bit_BootFileLength            resd 1
; bit_Checksum                  resd 1
; bit_Reserved                  resb 40

; where to load the kernel to
KERNEL_OFFSET equ 0x1000

; BIOS sets boot drive in 'dl'; store for later use
mov [BOOT_DRIVE], dl

; setup stack
mov bp, 0x9000
mov sp, bp

mov ah, 0x00
mov al, 0x03
int 10h

; mov ah, 0x00
; mov al, 0x13
; int 10h


call load_kernel

mov al,0x11        
out 0x20,al        
dw   0x00eb,0x00eb       
out 0xA0,al      
dw   0x00eb,0x00eb
mov al,0x20        
out 0x21,al
dw   0x00eb,0x00eb
mov al,0x28        
out 0xA1,al
dw   0x00eb,0x00eb
mov al, 0x04       
out 0x21,al
dw   0x00eb,0x00eb
mov al,0x02        
out 0xA1,al
dw   0x00eb,0x00eb
mov al,0x01       
out 0x21,al
dw   0x00eb,0x00eb
out 0xA1,al
dw   0x00eb,0x00eb
mov al,0xFF       
out 0x21,al
dw   0x00eb,0x00eb
out 0xA1,al

call switch_to_32bit



%include "disk.asm"
%include "gdt.asm"
%include "switch-to-32bit.asm"

[bits 16]
load_kernel:

    mov bx, KERNEL_OFFSET ; bx -> destination
    mov dh, 2             ; dh -> num sectors
    mov dl, [BOOT_DRIVE]  ; dl -> disk
    call disk_load
    ret

[bits 32]
BEGIN_32BIT:

    call KERNEL_OFFSET ; give control to the kernel
    jmp $ ; loop in case kernel returns

; boot drive variable
BOOT_DRIVE db 0

; padding
times 510 - ($-$$) db 0

; magic number
dw 0xaa55