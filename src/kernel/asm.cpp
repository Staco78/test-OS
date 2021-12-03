#include "asm.h"

void write_port(uint16 port, uint16 value)
{
    __asm__ volatile("outb %%al,%%dx" ::"a"(value), "d"(port));
}

uint16 read_port(uint16 port)
{
    uint16 _v;
    __asm__ volatile("inb %%dx,%%al"
                     : "=a"(_v)
                     : "d"(port));
    return _v;
}
