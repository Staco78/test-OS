#include "terminal.h"
#include "idt.h"
#include "memory.h"
#include "fs.h"
#include "tss.h"
#include "processes.h"
#include "clock.h"
#include "panic.h"

void main(uint32 gdtAddress)
{
    terminalInit();
    idt_assemble();
    Memory::init();
    Fs::init();
    write_tss(gdtAddress);
    Scheduler::init();
    Clock::init();

    // __asm__ volatile("xchgw %bx, %bx");

    create_process("/program");
    create_process("/program1");
}
