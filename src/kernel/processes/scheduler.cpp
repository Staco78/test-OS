#include "processes.h"
#include "terminal.h"
#include "lib/memoryUtils.h"

namespace Scheduler
{
    Process *current;
    ProcessesQueue queue;

    void init()
    {
        queue = ProcessesQueue();
    }

    void schedule(Registers *regs)
    {
        if (!current)
        {
            print("Not current process, schedule ignored\n");
            return;
        }

        memcpy(&current->regs, regs, sizeof(Registers));

        queue.push(current);
        current = queue.pop();

        memcpy(regs, &current->regs, sizeof(Registers));

        Memory::Pages::switchDirectory(&current->pagingDirectory);

        if (!current->started)
        {
            current->started = true;
            regs->eip = current->entry;
            if (regs->cs != 0x1B)
            {
                regs->cs = 0x1B;
                regs->ss = 0x23;
            }
        }
    }

    void add(Process *process)
    {
        queue.push(process);
        if (queue.getSize() == 1)
            current = process;
    }

    void setCurrent(Process *process)
    {
        current = process;
    }
} // namespace Scheduler
