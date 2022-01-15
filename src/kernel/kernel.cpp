#include "terminal.h"
#include "idt.h"
#include "memory.h"
#include "fs.h"
#include "tss.h"

extern "C" void jump_userMode();

extern "C" void user_func()
{

    // __asm__ volatile("xchgw %bx, %bx");
    while (1)
    {
        __asm__("int $0x80");
    }
}

void main(uint32 gdtAddress)
{
    terminalInit();
    idt_assemble();
    Memory::init();

    write_tss(gdtAddress);

    jump_userMode();
}
