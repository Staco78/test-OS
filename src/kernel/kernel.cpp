#include "terminal.h"
#include "idt.h"
#include "memory.h"
#include "fs.h"
#include "tss.h"
#include "processes.h"
#include "clock.h"
#include "panic.h"
#include "debug.h"
#include "syscalls.h"

void main(uint32 gdtAddress)
{
    terminalInit();
    idt_assemble();
    Memory::init();
    Fs::init();
    write_tss(gdtAddress);
    Syscalls::init();
    Scheduler::init();

    create_process("/program");
    create_process("/program1");
    Clock::init();
}
