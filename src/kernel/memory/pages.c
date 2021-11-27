#include "memory.h"

uint32 *page_directory;

void remap_to_itself(uint32 *address);

uint32 *createTable(uint16 number)
{
    uint32 *table = kmalloc();
    for (uint16 i = 0; i < 1024; i++)
    {
        *(table + i) = 0;
    }
    *(page_directory + number) = (uint32)table | 3;
    remap_to_itself(table);
    return (uint32 *)(*(page_directory + number) & 0xFFFFF000);
}

void remap_to_itself(uint32 *address)
{

    if ((uint32)address % 4096 != 0)
        panic("Invalid address to remap");

    uint32 pageNumber = (uint32)address / 4096;
    uint16 tableNumber = pageNumber / 1024;
    uint16 localPageNumber = pageNumber % 1024;

    uint32 *table = (uint32 *)(*(page_directory + tableNumber) & 0xFFFFF000);

    if (table == 0)
    {
        table = createTable(tableNumber);
    }

    *(table + localPageNumber) = (uint32)address | 3;
}

void pages_init()
{
    page_directory = kmalloc();

    for (uint16 i = 0; i < 1024; i++)
    {
        *(page_directory + i) = 0;
    }

    uint32 *table0 = createTable(0);

    for (uint16 i = 0; i < 1024; i++)
    {
        *(table0 + i) = (i * 4096) | 3;
    }

    __asm__ volatile("mov %0, %%cr3" ::"r"(page_directory));
    __asm__ volatile("mov %%cr0, %%eax\n\r"
                     "or $0x80000000, %%eax\n\r"
                     "mov %%eax, %%cr0" ::);
}