#include "memory.h"

uint32 *page_directory;

void pages_init()
{
    page_directory = kmalloc();

    for (uint16 i = 0; i < 1024; i++)
    {
        *(page_directory + i) = 0;
    }

    uint32 *page0 = kmalloc();

    *(page_directory) = (uint32)page0 | 3;

    for (uint16 i = 0; i < 1024; i++)
    {
        *(page0 + i) = (i * 4096) | 3;
    }

    uint32 *page768 = kmalloc();

    *(page_directory + 768) = (uint32)page768 | 3;

    for (uint16 i = 0; i < 1024; i++)
    {
        *(page768 + i) = (0x1000 + 4096 * i) | 3;
    }

    __asm__ volatile("mov %0, %%cr3" ::"r"(page_directory));
    __asm__ volatile("mov %%cr0, %%eax\n\r"
                     "or $0x80000000, %%eax\n\r"
                     "mov %%eax, %%cr0" ::);
}