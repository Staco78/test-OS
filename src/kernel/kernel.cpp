#include "terminal.h"
#include "idt.h"
#include "memory.h"
#include "fs.h"
#include "tss.h"
#include "processes.h"

void main(uint32 gdtAddress)
{
    terminalInit();
    idt_assemble();
    Memory::init();
    Fs::init();
    write_tss(gdtAddress);

    create_process("/program");
}
