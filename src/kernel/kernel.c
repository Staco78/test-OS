#include "terminal.h"
#include "idt.h"
#include "memory.h"

void main()
{
    terminalInit();
    idt_assemble();

    memory_init();
}
