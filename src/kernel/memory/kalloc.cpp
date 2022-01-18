#include "memory.h"
#include "panic.h"
#include "terminal.h"

namespace Memory
{
    namespace KernelAlloc
    {
#define HEAP_SIZE 4194304 // 4 Mio

        struct allocated_header
        {
            uint32 size;
            uint8 isFree;
            struct allocated_header *next;
        };

        typedef struct allocated_header allocated_header;

        allocated_header *heap_head;

        void init()
        {
            uint32 address = Physical::get_free_pages_aligned(1024, 4096 * 1024);
            Pages::kernel_map_table(address / (4096 * 1024), address + 0xC0000000);

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
    } // namespace KernelAlloc

    void init()
    {
        Physical::init();
        Pages::init();
        KernelAlloc::init();
    }

} // namespace Memory
