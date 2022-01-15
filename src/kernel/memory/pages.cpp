#include "memory.h"
#include "panic.h"

namespace Memory
{
    namespace Pages
    {
        uint32 **paging_directory;
        uint32 **paging_tables;
        uint32 **paging_tables_physical;

        void init()
        {
            paging_directory = (uint32 **)0xC0100000;
            paging_tables = (uint32 **)0xC0101000;
            paging_tables_physical = (uint32 **)0x00101000;
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
    } // namespace Pages

} // namespace Memory
