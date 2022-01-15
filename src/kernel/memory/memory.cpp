#include "memory.h"
#include "panic.h"
#include "terminal.h"

struct memory_map
{
    uint32 address;
    uint32 length;
    uint32 type;
};

void alloc_init();

struct memory_map memory_maps[6];

uint8 *memory_bitmap;

uint32 **paging_directory;
uint32 **paging_tables;
uint32 **paging_tables_physical;

void bitmap_set_1(uint32 index)
{
    uint8 *bitmap = &memory_bitmap[index / 8];
    *bitmap = *bitmap | (0b10000000 >> (index % 8));
}

void bitmap_set_0(uint32 index)
{
    uint8 *bitmap = &memory_bitmap[index / 8];
    *bitmap = *bitmap & ~(0b10000000 >> (index % 8));
}

int bitmap_get(uint32 index)
{
    uint8 *bitmap = &memory_bitmap[index / 8];
    return *bitmap & (0b10000000 >> (index % 8));
}

// return physical address of count free pages
uint32 bitmap_find_free_pages(int count, uint32 alignment)
{
    uint32 index;
    int size = 0;
    for (uint32 i = 0; i < 131072 * 8; i++)
    {
        if (bitmap_get(i) == 0)
        {
            if (size == 0)
            {
                if ((i * 4096) % alignment == 0)
                    index = i;
                else
                    continue;
            }
            size++;
            if (size == count)
                return index * 4096;
        }
        else
            size = 0;
    }
    panic("Physical memory allocator: cannot find free pages");
    return 0;
}

// return physical address of count free pages and mark they used
uint32 bitmap_get_free_pages_aligned(int count, uint32 alignment)
{
    if (alignment % 4096 != 0)
        panic("Physical memory allocato: invalid alignment");
    uint32 address = bitmap_find_free_pages(count, alignment);
    for (uint32 i = address / 4096; i < address / 4096 + count; i++)
        bitmap_set_1(i);
    return address;
}

// return physical address of count free pages and mark they used
uint32 bitmap_get_free_pages(int count)
{
    return bitmap_get_free_pages_aligned(count, 4096);
}

void createPagingTable(uint16 tableIndex)
{
    paging_directory[tableIndex] = (uint32 *)(((uint32)(paging_tables_physical + tableIndex * 1024) & 0xFFFFF000) | 7);

    for (uint16 i = 0; i < 1024; i++)
    {
        paging_tables[tableIndex * 1024 + i] = 0;
    }
}

// remap 4 Kio from baseAddress to virtualAddress
void map_page(uint32 virtualAddress, uint32 physicalAddress)
{
    if (virtualAddress % 4096 != 0)
    {
        panic("Invalid address to remap");
    }

    if (physicalAddress % 4096 != 0)
        panic("Invalid address to remap");

    if (paging_directory[virtualAddress / 4194304] == 0) // 4194304 = 4096 * 1024
    {
        createPagingTable(virtualAddress / 4194304);
    }

    paging_tables[virtualAddress / 4096] = (uint32 *)((physicalAddress & 0xFFFFF000) | 7);
}

void unmap_page(uint32 baseAddress)
{
    paging_tables[baseAddress / 4096] = 0;
}

void map_table(uint16 tableIndex, uint32 virtualAddress)
{

    if (virtualAddress % 4096 != 0)
        panic("Invalid address to remap");

    if (paging_directory[tableIndex] == 0)
    {
        createPagingTable(tableIndex);
    }

    for (uint16 i = 0; i < 1024; i++)
    {
        map_page(virtualAddress + i * 4096, (tableIndex * 1024 + i) * 4096);
    }
}

void memory_init()
{
    memory_bitmap = (uint8 *)0xC0500000;
    uint8 *ptr = (uint8 *)0xC0008000;
    for (uint8 i = 0; i < 6; i++)
    {
        memory_maps[i].address = (ptr[24 * i + 4]) + (ptr[24 * i + 5] << 8) + (ptr[24 * i + 6] << 16) + (ptr[24 * i + 7] << 24);
        memory_maps[i].length = (ptr[24 * i + 12]) + (ptr[24 * i + 13] << 8) + (ptr[24 * i + 14] << 16) + (ptr[24 * i + 15] << 24);
        memory_maps[i].type = (ptr[24 * i + 20]) + (ptr[24 * i + 21] << 8) + (ptr[24 * i + 22] << 16) + (ptr[24 * i + 23] << 24);
    }

    // __asm__("xchgw %bx, %bx");

    // fill bitmap with 1
    for (int i = 0; i < 131072; i++)
    {
        memory_bitmap[i] = 0xFF;
    }
    // __asm__("xchgw %bx, %bx");

    for (uint8 i = 0; i < 6; i++)
    {
        if (memory_maps[i].type == 1)
        {
            uint16 n = memory_maps[i].length / 4096;
            for (int j = memory_maps[i].address / 4096; j < n + (memory_maps[i].address / 4096); j++)
            {
                if (j * 4096 >= 0x100000)
                {
                    bitmap_set_0(j);
                }
            }
        }
    }

    for (int i = 32; i < 164; i++)
    {
        memory_bitmap[i] = 0xFF;
    }

    paging_directory = (uint32 **)0xC0100000;
    paging_tables = (uint32 **)0xC0101000;
    paging_tables_physical = (uint32 **)0x00101000;

    alloc_init();
    // __asm__("xchgw %bx, %bx");

    // __asm__ volatile("mov %0, %%cr3" ::"r"(paging_directory));
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
    // __asm__("xchgw %bx, %bx");
    uint32 address = bitmap_get_free_pages_aligned(1024, 4096 * 1024);
    map_table(address / (4096 * 1024), address + 0xC0000000);

    // __asm__ volatile("mov %cr3, %eax\n"
    //  "mov %eax, %cr3");

    heap_head = (allocated_header *)(address + 0xC0000000);
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
    // print("B at ");
    // printHex(((uint32)current + sizeof(allocated_header)));
    // printLn();
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
    // print("B\n");
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
    print("B / ");
    printInt(HEAP_SIZE);
    print("B)\n");
}