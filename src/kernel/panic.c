#include "panic.h"

void panic(uint8 *str)
{
    print("Kernel panic: ");
    print(str);
    __asm__ volatile("cli");
    while (1)
    {
        ;
    }
}