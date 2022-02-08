#include "processes.h"
#include "terminal.h"
#include "lib/memoryUtils.h"

#include "memory.h"

namespace Scheduler
{
    Process *current;
    ProcessesQueue queue;
    Registers *return_regs;

    void init()
    {
        queue = ProcessesQueue();
    }

    void switch_to_next()
    {
        current = queue.pop();

        memcpy(return_regs, &current->regs, sizeof(Registers));

        Memory::Pages::switchDirectory(&current->pagingDirectory);

        if (!current->started)
        {
            current->started = true;
            return_regs->eip = current->entry;
            if (return_regs->cs != 0x1B)
            {
                return_regs->cs = 0x1B;
                return_regs->ss = 0x23;
            }
        }
    }

    void schedule()
    {
        if (!current)
        {
            print("Not current process, schedule ignored\n");
            return;
        }

        memcpy(&current->regs, return_regs, sizeof(Registers));

        queue.push(current);
        switch_to_next();
    }

    void add(Process *process)
    {
        if (!current)
            current = process;
        else
            queue.push(process);
    }

    void exit_current()
    {
        Memory::KernelAlloc::kfree(current);
        switch_to_next();
    }

} // namespace Scheduler
