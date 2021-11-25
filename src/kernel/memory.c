#include "memory.h"
#include "terminal.h"

__attribute__((aligned(4096))) uint32 page_directory[1024];

void memory_init()
{
    for (uint16 i = 0; i < 1024; i++)
    {
        page_directory[i] = 0x00000002;
    }

    uint32 first_page_table[1024] __attribute__((aligned(4096)));

    for (uint16 i = 0; i < 1024; i++)
    {
        first_page_table[i] = (i * 0x1000) | 3;
    }

    page_directory[0] = ((uint16)first_page_table) | 3;

    __asm__ volatile("mov %0, %%cr3" ::"r"(page_directory[0]));
    __asm__ volatile("mov %%cr0, %%eax\n\r"
                     "or $0x8000, %%eax\n\r"
                     "mov %%eax, %%cr0" ::);

}