#include "types.h"
#include "asm.h"
#include "terminal.h"

void insw(uint16 port, void *addr, uint32 index)
{
    __asm__ volatile("cli");
    __asm__ volatile("rep; ins"
                     "w"
                     : "+D"(addr), "+c"(index)
                     : "d"(port));
    __asm__ volatile("sti");
}

void outsw(uint16 port, void *addr, uint32 index)
{
    __asm__ volatile("cli");
    __asm__ volatile("rep; outs"
                     "w"
                     : "+D"(addr), "+c"(index)
                     : "d"(port));
    __asm__ volatile("sti");
}

// #define inb_p(port) (                      \
//     {                                      \
//         unsigned char _v;                  \
//         __asm__ volatile("inb %%dx,%%al\n" \
//                          "\tjmp 1f\n"      \
//                          "1:\tjmp 1f\n"    \
//                          "1:"              \
//                          : "=a"(_v)        \
//                          : "d"(port));     \
//         _v;                                \
//     })

void fs_start()
{

    // printBin(read_port(0x1F7));
    // printLn();

    // write_port(0x1F6, 0x0);
    // write_port(0x1F7, 0x0EC);

    // printBin(read_port(0x1F7));
    // printLn();
    // printBin(read_port(0x1F1));
    // printLn();

    // uint8 buff[256] = {0};

    // insw(0x1F0, buff, 256);

    // for (uint16 i = 0; i < 255; i++)
    // {
    //     printChar(buff[i]);
    // }

    uint8 buff2[256];

    for (uint8 i = 0; i < 255; i++)
    {
        buff2[i] = 0;
    }

    write_port(0x1F6, 0);
    write_port(0x1F2, 1);
    write_port(0x1F3, 1);
    write_port(0x1F4, 0);
    write_port(0x1F5, 0);
    write_port(0x1F7, 0x30);

    while (!(read_port(0x1F7) & 8))
    {
    }

    printBin(read_port(0x1F7));
    printLn();

        outsw(0x1F0, buff2, 256);

    while (read_port(0x1F7) & 0b10000000)
    {
        ;
    }

    printBin(read_port(0x1F7));
    printLn();

    // READ

    write_port(0x1F6, 0);
    write_port(0x1F2, 1);
    write_port(0x1F3, 1);
    write_port(0x1F4, 0);
    write_port(0x1F5, 0);
    write_port(0x1F7, 0x20);

    while (!(read_port(0x1F7) & 8))
    {
    }

    printBin(read_port(0x1F7));
    printLn();

    uint8 buff[256] = {0};

    insw(0x1F0, buff, 256);

    printBin(read_port(0x1F7));
    printLn();

    for (uint16 i = 0; i < 255; i++)
    {
        printHex((uint8)buff[i]);
    }
}

void hd_interrupt(void)
{
    print("interrupt");
}