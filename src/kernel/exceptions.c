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