#include "clock.h"
#include "asm.h"
#include "terminal.h"

namespace Clock
{
    void init()
    {
        __asm__ volatile("cli");

        int rate = 3;
        rate &= 0x0F;
        write_port(0x70, 0x8A);
        uint8 prev = read_port(0x71);
        write_port(0x70, 0x8A);
        write_port(0x71, (prev & 0xF0) | rate);

        write_port(0x70, 0x8B);
        prev = read_port(0x71);
        write_port(0x70, 0x8B);
        write_port(0x71, prev | 0x40);

        __asm__ volatile("sti");

        write_port(0x70, 0x0C);
        read_port(0x71);
    }
} // namespace Clock
