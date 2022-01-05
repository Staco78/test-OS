#include "memory.h"

struct memory_map
{
    uint32 address;
    uint32 length;
    uint32 type;
};

void alloc_init();

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
    pages_stack[stack_index] = (uint32 *)address;
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

// remap 4 Kio from baseAddress to virtualAddress
void map_page(uint32 baseAddress, uint32 virtualAddress)
{
    if (baseAddress % 4096 != 0)
    {
        printHex(baseAddress);
        printLn();
        panic("Invalid address to remap");
    }

    if (virtualAddress % 4096 != 0)
        panic("Invalid address to remap");

    if (paging_directory[baseAddress / 4194304] == 0) // 4194304 = 4096 * 1024
    {
        createPagingTable(baseAddress / 4194304);
    }

    paging_tables[baseAddress / 4096] = (uint32 *)((virtualAddress & 0xFFFFF000) | 3);
}

void unmap_page(uint32 baseAddress)
{
    paging_tables[baseAddress / 4096] = 0;
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
        map_page((n * 1024 + i) * 4096, address + i * 4096);
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

    map_page((uint32)paging_directory * 4, (uint32)paging_directory);

    for (uint16 i = 0; i < 1024; i++)
    {
        map_page(((uint32)paging_tables / 1024 + i) * 4096, (uint32)(paging_tables + 1024 * i));
    }

    map_table(0, 0);
    map_table(1, 4096 * 1024);
    map_table(2, 4096 * 1024 * 2);

    for (uint16 i = 0; i < 1024; i++)
    {
        map_page(((uint32)pages_stack / 1024 + i) * 4096, (uint32)(pages_stack + 1024 * i));
    }

    __asm__ volatile("mov %0, %%cr3" ::"r"(paging_directory));
    __asm__ volatile("mov %%cr0, %%eax\n\r"
                     "or $0x80000000, %%eax\n\r"
                     "mov %%eax, %%cr0" ::);

    alloc_init();
}

#define HEAP_SIZE 4194304 // 4 Mio

struct allocated_header
{
    uint32 size;
    uint8 isFree;
    struct allocated_header *next;
};

typedef struct allocated_header allocated_header;

allocated_header *heap_head;

void alloc_init()
{
    map_table(3, 4096 * 1024 * 3);
    heap_head = (allocated_header *)(4096 * 1024 * 3);
    heap_head->isFree = 1;
    heap_head->next = 0;
    heap_head->size = HEAP_SIZE;
}

void *kmalloc(uint32 size)
{
    //find free node
    allocated_header *current = heap_head;
    while (current != 0)
    {
        if (current->isFree == 1 && current->size >= size)
            goto found;
        if ((uint32)current + sizeof(allocated_header) > HEAP_SIZE + (uint32)heap_head)
            break;
        current = current->next;
    }
    panic("kmalloc: Out of memory");

found:
    // print("alloc ");
    // printInt(size);
    // print("o\n");
    if (current->size - size <= sizeof(allocated_header) + 1)
    {
        current->isFree = 0;
        return (void *)((uint32)current + sizeof(allocated_header));
    }

    allocated_header *new_header = (allocated_header *)((uint32)current + size + sizeof(allocated_header));
    new_header->isFree = 1;
    new_header->size = current->size - (size + sizeof(allocated_header));
    new_header->next = current->next;

    current->isFree = 0;
    current->next = new_header;
    current->size = size;
    return (void *)((uint32)current + sizeof(allocated_header));
}

void kfree(void *address)
{

    uint32 header_address = (uint32)address - sizeof(allocated_header);

    allocated_header *current = heap_head;

    while (current != 0)
    {
        if (current->isFree == 0 && (uint32)current == header_address)
            goto found;
        else
            current = current->next;
    }
    panic("kfree: Unable to free: not found");

found:
    // print("free ");
    // printInt(current->size);
    // print("o\n");
    current->isFree = 1;

    if ((uint32)current->next == 0)
        return;
    if (current->next->isFree == 0)
        return;

    current->size += current->next->size + sizeof(allocated_header);
    current->next = current->next->next;
}

void printMemoryUsage()
{
    uint32 used = 0;
    for (allocated_header *current = heap_head; current != 0; current = current->next)
    {
        if (current->isFree == 0)
            used += current->size + 12;
    }

    print("Kernel memory usage: ");
    printInt((uint32)((float)used / (float)HEAP_SIZE));
    printChar('%');
    print(" (");
    printInt(used);
    print("b / ");
    printInt(HEAP_SIZE);
    print("b)\n");
}