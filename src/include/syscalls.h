#pragma once
#include "processes.h"

#define SYSCALLS_COUNT 2

namespace Syscalls
{
    void init();
    extern uint32 funcs[SYSCALLS_COUNT];
} // namespace Syscalls
