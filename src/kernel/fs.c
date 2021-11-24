#include "fs.h"

void insw(uint16 port, void *addr, uint32 count)
{
    __asm__ volatile("cli");
    __asm__ volatile("rep; ins"
                     "w"
                     : "+D"(addr), "+c"(count)
                     : "d"(port));
    __asm__ volatile("sti");
}

void outsw(uint16 port, void *addr, uint32 count)
{
    __asm__ volatile("cli");
    __asm__ volatile("rep; outs"
                     "w"
                     : "+S"(addr), "+c"(count)
                     : "d"(port));
    __asm__ volatile("sti");
}

void waitDiskReady()
{
    while (read_port(0x1F7) & 0b10000000)
    {
        ;
    }
}

void fs_start()
{

    uint8 buff[512];

    for (uint32 i = 0; i < 512; i++)
    {
        buff[i] = 0x11;
    }

    writeDisk(0, 1, buff);

    waitDiskReady();

    uint8 buff2[512];

    readDisk(0, 1, buff2);

    for (uint16 i = 0; i < 512; i++)
    {
        print8bitHex(buff2[i]);
        if (i % 2 != 0)
            printChar(' ');
    }
}

void readDisk(uint32 address, uint16 nbSector, uint8 *buff)
{
    write_port(0x1F6, (address >> 24) | 0b11100000);
    write_port(0x1F2, nbSector);
    write_port(0x1F3, (uint8)address);
    write_port(0x1F4, (uint8)address >> 8);
    write_port(0x1F5, (uint8)address >> 16);
    write_port(0x1F7, 0x20);

    while (!(read_port(0x1F7) & 8))
    {
    }

    insw(0x1F0, buff, 256);
}

void writeDisk(uint32 address, uint16 nbSector, uint8 *buff)
{
    write_port(0x1F6, (address >> 24) | 0b11100000);
    write_port(0x1F2, nbSector);
    write_port(0x1F3, (uint8)address);
    write_port(0x1F4, (uint8)address >> 8);
    write_port(0x1F5, (uint8)address >> 16);
    write_port(0x1F7, 0x30);

    while (!(read_port(0x1F7) & 8))
    {
    }

    outsw(0x1F0, buff, 256);
}