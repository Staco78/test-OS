#include "memory.h"
#include "panic.h"
#include "terminal.h"
#include "debug.h"
#include "lib/memoryUtils.h"

#define VIRTUAL_DIRECTORY_ADDRESS 0xC0100000
#define VIRTUAL_TABLES_ADDRESS 0xC0101000

namespace Memory
{
    namespace Pages
    {

        Directory kernel_directory;

        void init()
        {
            kernel_directory.physicalAddress = 0x100000;
            kernel_directory.tablesPhysical = (DirectoryEntry *)VIRTUAL_DIRECTORY_ADDRESS;
            kernel_directory.tablesAddress = (TableEntry *)VIRTUAL_TABLES_ADDRESS;

            // kernel_directory.tablesAddress[0] = (TableEntry *)0xC0102000;
            // kernel_directory.tablesAddress[768] = (TableEntry *)0xC0103000;
            // kernel_directory.tablesAddress[769] = (TableEntry *)0xC0104000;
        }

        void kernel_createPagingTable(uint16 tableIndex)
        {
            uint32 physicalAddress = Physical::get_free_pages(1);
            kernel_directory.tablesPhysical[tableIndex].address = physicalAddress >> 12;
            kernel_directory.tablesPhysical[tableIndex].write = 1;
            kernel_directory.tablesPhysical[tableIndex].present = 1;
            uint32 virtualAddress = (uint32)kernel_directory.tablesAddress + (tableIndex * 4096);
            kernel_map_page(virtualAddress, physicalAddress);
            memset((void *)virtualAddress, 0x00, 4096);

            // kernel_directory.tablesAddress[tableIndex] = (TableEntry *)virtualAddress;
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

            if (kernel_directory.tablesPhysical[virtualAddress / 4194304].present == 0) // 4194304 = 4096 * 1024
            {
                kernel_createPagingTable(virtualAddress / 4194304);
            }
            uint32 pageIndex = virtualAddress / 4096;
            TableEntry *table = &kernel_directory.tablesAddress[pageIndex];
            table->address = physicalAddress >> 12;
            table->write = 1;
            table->present = 1;
        }

        uint32 kernel_get_free_page()
        {
            static uint16 table_to_use = 0;
            uint16 first_free_table = 0;

            if (table_to_use != 0)
            {
                for (uint16 i = 0; i < 1024; i++)
                {
                    if (kernel_directory.tablesAddress[table_to_use * 1024 + i].present == 1)
                        continue;

                    uint32 address = (table_to_use * 1024 + i) * 4096;
                    if (i == 1023)
                        table_to_use = 0;
                    return address;
                }
            }

            for (uint16 i = 770; i < 1024; i++)
            {
                if (kernel_directory.tablesPhysical[i].present = 0)
                {
                    if (first_free_table == 0)
                        first_free_table = i;
                    continue;
                }
                for (uint16 j = 0; j < 1024; j++)
                {
                    if (kernel_directory.tablesAddress[i * 1024 + j].present == 1)
                        continue;

                    if (j < 1023)
                        table_to_use = i;
                    return (i * 1024 + j) * 4096;
                }
            }

            if (first_free_table == 0)
                panic("Kernel get free page: not enought memory");

            kernel_createPagingTable(first_free_table);
            return first_free_table * 1024 * 4096;
        }

        void kernel_unmap_page(uint32 virtualAddress)
        {
            panic("Not implemented yet");
            // kernel_directory.tablesAddress[virtualAddress / 4194304][(virtualAddress / 4194304) % 1024].present = 0;
        }

        void kernel_map_table(uint32 physicalAddress, uint32 virtualAddress)
        {
            if (physicalAddress % (4096 * 1024) != 0)
                panic("Invalid address to remap");

            if (virtualAddress % 4096 != 0)
                panic("Invalid address to remap");

            if (kernel_directory.tablesPhysical[virtualAddress / (4096 * 1024)].present == 0)
            {
                kernel_createPagingTable(virtualAddress / (4096 * 1024));
            }

            for (uint16 i = 0; i < 1024; i++)
            {
                kernel_map_page(virtualAddress + i * 4096, physicalAddress + i * 4096);
            }
        }

        void createPagingTable(Directory *directory, uint32 index)
        {
            uint32 physicalAddress = Physical::get_free_pages(1);
            directory->tablesPhysical[index].address = physicalAddress >> 12;
            directory->tablesPhysical[index].write = 1;
            directory->tablesPhysical[index].user = 1;
            directory->tablesPhysical[index].present = 1;
            // directory->tablesAddress[index] = (TableEntry *)((index + 2) * 4096);
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

            if (directory->tablesAddress[virtualAddress / 4194304].present == 0) // 4194304 = 4096 * 1024
            {
                createPagingTable(directory, virtualAddress / 4194304);
            }

            // ((uint32 **)directory->tablesAddress)[virtualAddress / 4194304][(virtualAddress / 4096) % 1024] = physicalAddress | 7;
            directory->tablesAddress[virtualAddress / 4096].address = physicalAddress >> 12;
            directory->tablesAddress[virtualAddress / 4096].write = 1;
            directory->tablesAddress[virtualAddress / 4096].user = 1;
            directory->tablesAddress[virtualAddress / 4096].present = 1;
        }

        void create_directory(Directory *directory)
        {
            // directory->physicalAddress = Physical::get_free_pages(1);
            // directory->virtualTablesPhysicalAddress = Physical::get_free_pages(1);
            // directory->tablesPhysical = 0;
            // directory->tablesAddress = 0x1000;
            // kernel_map_page(0, directory->physicalAddress);
            // kernel_map_page(0x1000, directory->virtualTablesPhysicalAddress);

            // directory->tablesPhysical[0].address = (Physical::get_free_pages(1) >> 12);
            // directory->tablesPhysical[0].present = 1;
            // directory->tablesPhysical[0].write = 1;
            // directory->tablesPhysical[0].user = 1;
            // directory->tablesAddress[0] = 0x2000;
            // kernel_map_page(0x2000, directory->tablesPhysical[0].address << 12);
            // map_page(directory, 0, directory->physicalAddress);
            // map_page(directory, 0x1000, directory->virtualTablesPhysicalAddress);
            // map_page(directory, 0x2000, directory->tablesPhysical[0].address << 12);

            // kernel_unmap_page(0);
            // kernel_unmap_page(0X1000);
            // kernel_unmap_page(0X2000);



            // directory->physicalAddress = Physical::get_free_pages(1);
            // map_page(directory, VIRTUAL_DIRECTORY_ADDRESS, directory->physicalAddress);
            // createPagingTable(directory, 768);
            // createPagingTable(directory, 769);

            panic("Not implemented");
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
                to->tablesPhysical[i] = kernel_directory.tablesPhysical[i];
            }
            kernel_unmap_page((uint32)to->tablesPhysical);
        }
    } // namespace Pages

} // namespace Memory