#include "idt.h"
#include "asm.h"
#include "terminal.h"

void fs_start();

void main()
{
    terminalInit();
    idt_assemble();

    fs_start();
}
