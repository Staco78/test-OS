#include "syscalls.h"
#include "terminal.h"

#include "debug.h"

#include "errno.h"

namespace Syscalls
{
    uint32 funcs[SYSCALLS_COUNT];

    // int write(void* ptr, uint32 size);
    int write(const char *str, uint32 len)
    {
        if (!Memory::Pages::isUserPtrValid((uint32)str, len, false))
            return -EFAULT;
        print(str, len);
        return 0;
    }

    int exit(uint32 code)
    {
        Scheduler::exit_current();
        return 0; // TODO: unreachable
    }

    void init()
    {
        funcs[0] = (uint32)write;
        funcs[1] = (uint32)exit;
    }

} // namespace Syscalls
