#pragma once
#include "terminal.h"

static void _panic(const char *str, const char *file, uint32 line)
{
    print("Kernel panic: ");
    print(str);
    printLn();
    print(file);
    printChar(' ');
    printInt(line);
    __asm__ volatile("cli");
    while (1)
    {
    }
}

#define panic(str) _panic(str, __FILE__, __LINE__);
