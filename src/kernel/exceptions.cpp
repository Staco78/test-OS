#include "panic.h"
#include "terminal.h"
#include "types.h"

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
void _13(uint32 e)
{
    if (e & 1)

        print("external ");

    else
        print("internal ");
    uint8 tbl = ((uint8)(e >> 1)) & 0b11;
    if (tbl == 0)

        print("GTD ");

    else if (tbl == 1 || tbl == 3)

        print("IDT ");

    else if (tbl == 2)

        print("LDT ");

    else
    {
        print("Invalid tbl value (");
        printInt(tbl);
        print(") ");
    }
    print("Index: ");
    printInt((uint16)(e >> 3));
    printLn();
    panic("General Protection Fault");
}
void _14(uint32 e)
{
    uint32 address;
    __asm__ volatile("mov %%cr2, %0"
                     : "=r"(address));

    // Output an error message.
    print("Page fault! ( ");
    if (!(e & 0x01))
    {
        if (e & 0x2)
        {
            print("read-only ");
        }
        if (e & 0x4)
        {
            print("user-mode ");
        }
        if (e & 0x8)
        {
            print("reserved ");
        }
        if (e & 0x20)
        {
            print("instruction-fetch ");
        }
        if (e & 0x40)
        {
            print("protection-key-violation ");
        }
        if (e & 0x80)
        {
            print("shadow-stack-access");
        }
    }
    else
        print("not present ");

    print(") at 0x");
    printHex(address);
    print("\n");
    panic("Page fault");
}