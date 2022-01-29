#pragma once

#define BOCHS_BREAKPOINT() \
    __asm__ volatile("xchgw %bx, %bx");
