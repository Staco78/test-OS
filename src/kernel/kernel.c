#include "idt.h"
#include "terminal.h"

void main()
{
    terminalInit();
    idt_assemble();
}
