#pragma once

#define panic(str)               \
    {                            \
        print("Kernel panic: "); \
        print(str);              \
        printLn();               \
        print(__FILE__);         \
        printChar(' ');          \
        printInt(__LINE__);      \
        __asm__ volatile("cli"); \
        while (1)                \
        {                        \
        }                        \
    }
