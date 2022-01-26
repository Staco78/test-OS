#pragma once
#include "types.h"

namespace Memory
{

    namespace Physical
    {
        void init();

        // return physical address of count free pages
        uint32 find_free_pages(int count, uint32 alignment);

        // return physical address of count free pages and mark they used
        uint32 get_free_pages_aligned(int count, uint32 alignment);

        // return physical address of count free pages and mark they used
        uint32 get_free_pages(int count);

    } // namespace Physical

    namespace Pages
    {

        struct Directory
        {
            uint32 physicalAddress;
            uint32 virtualTablesPhysicalAddress;
            uint32 *tablesPhysical;
            uint32 *tables;
        };

        void init();

        void kernel_map_table(uint16 tableIndex, uint32 virtualAddress);

        // remap 4 Kio from physicalAddress to virtualAddress
        void kernel_map_page(uint32 virtualAddress, uint32 physicalAddress);

        uint32 kernel_get_free_page();

        // copy kernel paging directory to an other
        void copy_kernel_pages(Directory *to);

        // remap 4 Kio from physicalAddress to virtualAddress
        void map_page(Directory *directory, uint32 virtualAddress, uint32 physicalAddress);

        void alloc_pages(Directory *directory, uint32 count, uint32 virtualAddress);

        void create_directory(Directory *directory);

    } // namespace Pages

    namespace KernelAlloc
    {
        void init();
        void kfree(void *address);
        void *kmalloc(uint32 size);

        void printMemoryUsage();

    } // namespace KernelAlloc

    void init();

}