#include "memory.h"

struct memory_map
{
    uint32 address;
    uint32 length;
    uint32 type;
};

struct memory_map memory_maps[6];
uint32 stack_index;
__attribute__((aligned(4096))) uint32 *pages_stack[1024 * 1024];

__attribute__((aligned(4096))) uint32 *paging_directory[1024];
__attribute__((aligned(1048576))) uint32 *paging_tables[1024 * 1024];

void push_stack(uint32 address)
{
    stack_index++;
    if (stack_index >= 1024 * 1024)
    {
        panic("Memory stack overflow");
        return;
    }
    pages_stack[stack_index] = (void *)address;
}

void *pop_stack()
{
    if (stack_index == 0)
    {
        panic("Out of memory");
        return 0;
    }
    return pages_stack[stack_index--];
}

void initStack()
{
    stack_index = -1;
}

void createPagingTable(uint16 n)
{
    paging_directory[n] = (uint32 *)(((uint32)(paging_tables + n * 1024) & 0xFFFFF000) | 3);
    for (uint16 i = 0; i < 1024; i++)
    {
        paging_tables[n * 1024 + i] = 0;
    }
}

void map_page(uint32 n, uint32 address)
{
    if (address % 4096 != 0)
        panic("Invalid address to remap");

    if (paging_directory[n / 1024] == 0)
    {
        createPagingTable(n / 1024);
    }

    paging_tables[n] = (uint32 *)((address & 0xFFFFF000) | 3);
}

void unmap_page(uint32 n)
{
    paging_tables[n] = 0;
}

void map_table(uint16 n, uint32 address)
{

    if (address % 4096 != 0)
        panic("Invalid address to remap");

    if (paging_directory[n] == 0)
    {
        createPagingTable(n);
    }

    for (uint16 i = 0; i < 1024; i++)
    {
        map_page(n * 1024 + i, address + i * 4096);
    }
}

void memory_init()
{
    initStack();

    uint8 *ptr = (uint8 *)0x8000;
    for (uint8 i = 0; i < 6; i++)
    {
        memory_maps[i].address = (ptr[24 * i + 4]) + (ptr[24 * i + 5] << 8) + (ptr[24 * i + 6] << 16) + (ptr[24 * i + 7] << 24);
        memory_maps[i].length = (ptr[24 * i + 12]) + (ptr[24 * i + 13] << 8) + (ptr[24 * i + 14] << 16) + (ptr[24 * i + 15] << 24);
        memory_maps[i].type = (ptr[24 * i + 20]) + (ptr[24 * i + 21] << 8) + (ptr[24 * i + 22] << 16) + (ptr[24 * i + 23] << 24);
    }

    for (uint8 i = 0; i < 6; i++)
    {
        if (memory_maps[i].type == 1)
        {
            uint16 n = memory_maps[i].length / 4096;
            for (uint16 y = 0; y < n; y++)
            {
                push_stack((memory_maps[i].address + 4096 * y));
            }
        }
    }

    for (uint16 i = 0; i < 1024; i++)
    {
        paging_directory[i] = 0;
    }

    map_page((uint32)paging_directory / 1024, (uint32)paging_directory);

    for (uint16 i = 0; i < 1024; i++)
    {
        map_page((uint32)paging_tables / 1024 + i, (uint32)(paging_tables + 1024 * i));
    }

    map_table(0, 0);
    map_table(1, 4096 * 1024);
    map_table(2, 4096 * 1024 * 2);

    for (uint16 i = 0; i < 1024; i++)
    {
        map_page((uint32)pages_stack / 1024 + i, (uint32)(pages_stack + 1024 * i));
    }

    __asm__ volatile("mov %0, %%cr3" ::"r"(paging_directory));
    __asm__ volatile("mov %%cr0, %%eax\n\r"
                     "or $0x80000000, %%eax\n\r"
                     "mov %%eax, %%cr0" ::);
}

void *kmalloc()
{
    uint32 address = (uint32)pop_stack();
    map_page(address / 4096, address);
    printInt(address / 4096);
    printLn();
    return (void *)address;
}

void kfree(void *address)
{
    if ((uint32)address % 4096 != 0)
        panic("Unable to free page: invalid address");
    unmap_page((uint32)address / 4096);
}