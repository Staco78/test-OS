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

        struct DirectoryEntry
        {
            uint32 present : 1;
            uint32 write : 1;
            uint32 user : 1;
            uint32 write_through : 1;
            uint32 cache_disable : 1;
            uint32 accessed : 1;
            uint32 reserved : 1;
            uint32 page_size : 1;
            uint32 global : 1;
            uint32 available : 3;
            uint32 address : 20;
        } __attribute__((packed));

        struct TableEntry
        {
            uint32 present : 1;
            uint32 write : 1;
            uint32 user : 1;
            uint32 write_through : 1;
            uint32 cache_disable : 1;
            uint32 accessed : 1;
            uint32 dirty : 1;
            uint32 attribute : 1;
            uint32 global : 1;
            uint32 available : 3;
            uint32 address : 20;
        } __attribute__((packed));

        struct Directory
        {
            uint32 physicalAddress;
            DirectoryEntry *tablesPhysical;
            TableEntry *tablesAddress;
        };

        void init();

        void kernel_map_table(uint32 physicalAddress, uint32 virtualAddress);

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