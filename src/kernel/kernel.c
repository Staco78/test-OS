#include "memory.h"
#include "idt.h"
#include "asm.h"
#include "terminal.h"
#include "keyboard.h"

void fs_start();

void main()
{
    terminalInit();
    idt_assemble();
    memory_init();

    print("keyboard interrupt address: ");
    printHex((uint32)&keyboard_interrupt);
    printLn();

    // fs_start();
}
