#include "terminal.h"
#include "idt.h"
#include "memory.h"
#include "fs.h"
#include "tss.h"
#include "processes.h"
#include "clock.h"

void main(uint32 gdtAddress)
{
    terminalInit();
    idt_assemble();
    Memory::init();
    Clock::init();

    // Fs::init();
    // write_tss(gdtAddress);
}
