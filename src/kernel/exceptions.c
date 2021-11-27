#include "panic.h"

void _0()
{
    panic("Divide by zero");
}
void _1()
{
    panic("Debug");
}
void _2()
{
    panic("Non-maskable Interrupt");
}
void _3()
{
    panic("Breakpoint");
}
void _4()
{
    panic("Overflow");
}

void _5()
{
    panic("Bound Range Exceeded");
}

void _6()
{
    panic("Invalid Opcode");
}
void _7()
{
    panic("Device not Available");
}
void _8()
{
    panic("Double Fault");
}
void _9()
{
    panic("Coprocessor Segment Overrun");
}
void _10()
{
    panic("Invalid TSS");
}
void _11()
{
    panic("Segment Not Present");
}
void _12()
{
    panic("Stack-Segment Fault");
}
void _13()
{
    panic("General Protection Fault");
}
void _14(uint32 e)
{
    uint32 address;
    __asm__ volatile("mov %%cr2, %0"
                     : "=r"(address));

    int present = !(e & 0x1); // Page not present
    int rw = e & 0x2;         // Write operation?
    int us = e & 0x4;         // Processor was in user-mode?
    int reserved = e & 0x8;   // Overwritten CPU-reserved bits of page entry?
    int id = e & 0x10;        // Caused by an instruction fetch?

    // Output an error message.
    print("Page fault! ( ");
    if (present)
    {
        if (rw)
        {
            print("read-only ");
        }
        if (us)
        {
            print("user-mode ");
        }
        if (reserved)
        {
            print("reserved ");
        }
    }
    else
        print("not present ");

    print(") at 0x");
    printHex(address);
    print("\n");
    panic("Page fault");
}