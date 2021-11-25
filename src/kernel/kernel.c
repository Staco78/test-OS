#include "idt.h"
#include "asm.h"
#include "terminal.h"
#include "memory.h"

void fs_start();

void main()
{
    terminalInit();
    idt_assemble();

    memory_init();

    uint8 *ptr = (uint8 *)0x8000;

    uint8 i = 0;

    while (i < 10)
    {
        print8bitHex(ptr[7]);
        print8bitHex(ptr[6]);
        print8bitHex(ptr[5]);
        print8bitHex(ptr[4]);
        printChar(' ');

        print8bitHex(ptr[15]);
        print8bitHex(ptr[14]);
        print8bitHex(ptr[13]);
        print8bitHex(ptr[12]);
        printChar(' ');

        print8bitHex(ptr[23]);
        print8bitHex(ptr[22]);
        print8bitHex(ptr[21]);
        print8bitHex(ptr[20]);
        printLn();
        ptr += 24;
        i++;
    }

    print8bitHex(ptr[7]);
    print8bitHex(ptr[6]);
    print8bitHex(ptr[5]);
    print8bitHex(ptr[4]);
    printChar(' ');

    print8bitHex(ptr[15]);
    print8bitHex(ptr[14]);
    print8bitHex(ptr[13]);
    print8bitHex(ptr[12]);
    printChar(' ');

    print8bitHex(ptr[23]);
    print8bitHex(ptr[22]);
    print8bitHex(ptr[21]);
    print8bitHex(ptr[20]);
    // fs_start();
}
