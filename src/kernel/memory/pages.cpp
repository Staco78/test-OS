#include "memory.h"
#include "panic.h"
#include "terminal.h"

namespace Memory
{
    namespace Pages
    {
        uint32 **kernel_paging_directory;
        uint32 **kernel_paging_tables;
        uint32 **kernel_paging_tables_physical;

        void init()
        {
            kernel_paging_directory = (uint32 **)0xC0100000;
            kernel_paging_tables = (uint32 **)0xC0101000;
            kernel_paging_tables_physical = (uint32 **)0x00101000;
        }

        void kernel_createPagingTable(uint16 tableIndex)
        {
            kernel_paging_directory[tableIndex] = (uint32 *)(((uint32)(kernel_paging_tables_physical + tableIndex * 1024) & 0xFFFFF000) | 3);

            for (uint16 i = 0; i < 1024; i++)
            {
                kernel_paging_tables[tableIndex * 1024 + i] = 0;
            }
        }

        // remap 4 Kio from baseAddress to virtualAddress
        void kernel_map_page(uint32 virtualAddress, uint32 physicalAddress)
        {
            if (virtualAddress % 4096 != 0)
            {
                printLn();
                printHex(virtualAddress);
                printLn();
                panic("Invalid virtual address to remap");
            }

            if (physicalAddress % 4096 != 0)
            {
                printLn();
                printHex(physicalAddress);
                printLn();
                panic("Invalid physical address to remap");
            }

            if (kernel_paging_directory[virtualAddress / 4194304] == 0) // 4194304 = 4096 * 1024
            {
                kernel_createPagingTable(virtualAddress / 4194304);
            }

            kernel_paging_tables[virtualAddress / 4096] = (uint32 *)((physicalAddress & 0xFFFFF000) | 3);
        }

        uint32 kernel_get_free_page()
        {
            uint32 free_table = 0; // impossible value
            for (int i = 770; i < 1024; i++)
            {
                uint32 *table = (uint32 *)(((uint32)kernel_paging_directory[i] & 0xFFFFF000) + 0xC0000000);
                if ((uint32)table == 0)
                {
                    free_table = i;
                    continue;
                }

                for (int j = 0; j < 1024; j++)
                {
                    uint32 page = table[j];
                    if (page == 0)
                        return (i * 1024 + j) * 4096;
                }
            }

            if (free_table == 0)
                panic("Kernel get free page: not enought memory");

            kernel_createPagingTable(free_table);
            return free_table * 1024 * 4096;
        }

        void kernel_unmap_page(uint32 virtualAddress)
        {
            kernel_paging_tables[virtualAddress / 4096] = 0;
        }

        void kernel_map_table(uint16 tableIndex, uint32 virtualAddress)
        {

            if (virtualAddress % 4096 != 0)
                panic("Invalid address to remap");

            if (kernel_paging_directory[tableIndex] == 0)
            {
                kernel_createPagingTable(tableIndex);
            }

            for (uint16 i = 0; i < 1024; i++)
            {
                kernel_map_page(virtualAddress + i * 4096, (tableIndex * 1024 + i) * 4096);
            }
        }

        void createPagingTable(Directory *directory, uint32 index)
        {
            // print("createPaginTable ");
            // printInt(index);
            // printLn();
            uint32 physicalAddress = Physical::get_free_pages(1);
            directory->tablesPhysical[index] = physicalAddress | 7;
            directory->tables[index] = (index + 2) * 4096;
            map_page(directory, (index + 2) * 4096, physicalAddress);
        }

        void map_page(Directory *directory, uint32 virtualAddress, uint32 physicalAddress)
        {
            // print("map page\n");
            if (virtualAddress % 4096 != 0)
            {
                printLn();
                printHex(virtualAddress);
                printLn();
                panic("Invalid virtual address to remap");
            }

            if (physicalAddress % 4096 != 0)
            {
                printLn();
                printHex(physicalAddress);
                printLn();
                panic("Invalid physical address to remap");
            }

            if (directory->tables[virtualAddress / 4194304] == 0) // 4194304 = 4096 * 1024
            {
                createPagingTable(directory, virtualAddress / 4194304);
            }

            ((uint32 **)directory->tables)[virtualAddress / 4194304][(virtualAddress / 4096) % 1024] = physicalAddress | 7;
        }

        void create_directory(Directory *directory)
        {
            directory->physicalAddress = Physical::get_free_pages(1);
            directory->virtualTablesPhysicalAddress = Physical::get_free_pages(1);
            directory->tablesPhysical = 0;
            directory->tables = (uint32 *)0x1000;
            kernel_map_page(0, directory->physicalAddress);
            kernel_map_page(0x1000, directory->virtualTablesPhysicalAddress);

            directory->tablesPhysical[0] = Physical::get_free_pages(1) | 7;
            directory->tables[0] = 0x2000;
            kernel_map_page(0x2000, directory->tablesPhysical[0] & 0xFFFFF000);
            map_page(directory, 0, directory->physicalAddress);
            map_page(directory, 0x1000, directory->virtualTablesPhysicalAddress);
            map_page(directory, 0x2000, directory->tablesPhysical[0] & 0xFFFFF000);

            kernel_unmap_page(0);
            kernel_unmap_page(0X1000);
            kernel_unmap_page(0X2000);
        }

        void alloc_pages(Directory *directory, uint32 count, uint32 virtualAddress)
        {
            for (uint32 i = 0; i < count; i++)
            {
                map_page(directory, virtualAddress, Physical::get_free_pages(1));
                virtualAddress += 4096;
            }
        }

        void copy_kernel_pages(Directory *to)
        {
            kernel_map_page((uint32)to->tablesPhysical, to->physicalAddress);
            for (int i = 768; i < 1024; i++)
            {
                to->tablesPhysical[i] = (uint32)kernel_paging_directory[i];
            }
            kernel_unmap_page((uint32)to->tablesPhysical);
        }
    } // namespace Pages

} // namespace Memory