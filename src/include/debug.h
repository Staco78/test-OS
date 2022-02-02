#pragma once
#include "panic.h"

#define BOCHS_BREAKPOINT() __asm__ volatile("xchgw %bx, %bx");

#define ASSERT(expr)                                             \
    if (!(expr))                                                 \
    {                                                            \
        _panic("Assert failed (" #expr ")", __FILE__, __LINE__); \
    }
