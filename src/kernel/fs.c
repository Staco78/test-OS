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

void fs_start()
{
    uint8 buff[512];

    for (uint16 i = 0; i < 512; i++)
    {
        buff[i] = i;
    }

    writeDisk(0, 1, 1, 0, buff);
}

void readDisk(uint8 head, uint8 startSector, uint8 nbSector, uint16 cyl, uint8 *buff)
{
    write_port(0x1F6, head & 0b00001111);
    write_port(0x1F2, nbSector);
    write_port(0x1F3, startSector);
    write_port(0x1F4, (uint8)cyl);
    write_port(0x1F5, (uint8)cyl >> 8);
    write_port(0x1F7, 0x20);

    while (!(read_port(0x1F7) & 8))
    {
    }

    printBin(read_port(0x1F7));
    printLn();

    insw(0x1F0, buff, 256);
}

void writeDisk(uint8 head, uint8 startSector, uint8 nbSector, uint32 cyl, uint8 *buff)
{
    write_port(0x1F6, head & 0b00001111);
    write_port(0x1F2, nbSector);
    write_port(0x1F3, startSector);
    write_port(0x1F4, (uint8)cyl);
    write_port(0x1F5, (uint8)cyl >> 8);
    write_port(0x1F7, 0x30);

    while (!(read_port(0x1F7) & 8))
    {
    }

    printBin(read_port(0x1F7));
    printLn();

    outsw(0x1F0, buff, 256);
}