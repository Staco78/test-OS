#include "memory.h"
#include "terminal.h"

struct memory_map
{
    uint32 address;
    uint32 length;
    uint32 type;
};

struct memory_map memory_maps[6];

__attribute__((aligned(8192))) uint32 pages_stack[1024 * 1024];

uint32 stack_index = -1;

void push_stack(uint32 address)
{
    stack_index++;
    if (stack_index >= 1024 * 1024)
    {
        print("Memory stack overflow");
        return;
    }
    pages_stack[stack_index] = address;
}

uint32 pop_stack()
{
    if (stack_index == 0)
    {
        print("Pop stack error");
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
            print("n: ");
            printInt(n);
            printLn();
            for (uint16 i = 0; i < n; i++)
            {
                push_stack((memory_maps[i].address + 4096 * i));
            }
        }
    }
    // printInt(memory_maps[3].length / 4096);
    // push_stack((uint8 *)(memory_maps[3].address + 4096 * 32480));
    print("stack[0]: ");
    printHex((uint32)&pages_stack[0]);
    printLn();
    print("stack[stack_index]: ");
    printHex((uint32)&pages_stack[stack_index]);
    printLn();
}