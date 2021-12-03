#include "panic.h"

void panic(const char* str)
{
    print("Kernel panic: ");
    print(str);
    __asm__ volatile("cli");
    while (1)
    {
        ;
    }
}