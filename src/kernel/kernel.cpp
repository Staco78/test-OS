#include "terminal.h"
#include "idt.h"
#include "memory.h"
#include "fs.h"

void main()
{
    terminalInit();
    idt_assemble();

    memory_init();
}
