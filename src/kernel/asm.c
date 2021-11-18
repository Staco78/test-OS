#include "asm.h"

void write_port(uint8 port, uint8 value)
{
    __asm__ volatile("outb %%al,%%dx" ::"a"(value), "d"(port));
}

uint8 read_port(uint8 port)
{
    uint8 _v;
    __asm__ volatile("inb %%dx,%%al"
                     : "=a"(_v)
                     : "d"(port));
    return _v;
}
