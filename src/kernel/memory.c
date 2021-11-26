#include "memory.h"

struct memory_map
{
    uint32 address;
    uint32 length;
    uint32 type;
};

struct memory_map memory_maps[6];

__attribute__((aligned(8192))) void *pages_stack[1024 * 1024];

uint32 stack_index = -1;

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
    stack_index--;
    return pages_stack[stack_index + 1];
}

void initStack()
{
    stack_index = 0;
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
            for (uint16 i = 0; i < n; i++)
            {
                push_stack((memory_maps[i].address + 4096 * i));
            }
        }
    }
}

// return pointer to a 4096 bytes page
void *kmalloc()
{
    return pop_stack();
}

// mark page from address at free
void kfree(void *address)
{
    if ((uint32)address % 4096 != 0)
        panic("Unable to free page: invalid address");
    push_stack((uint32)address);
}