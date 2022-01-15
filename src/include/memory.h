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
        void init();

        void map_table(uint16 tableIndex, uint32 virtualAddress);
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