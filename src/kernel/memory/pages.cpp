#include "memory.h"
#include "panic.h"
#include "terminal.h"
#include "debug.h"
#include "lib/memoryUtils.h"

#define KERNEL_DIRECTORY_ADDRESS 0x100000
#define KERNEL_VIRTUAL_DIRECTORY_ADDRESS 0xC0100000
#define KERNEL_VIRTUAL_TABLES_ADDRESS 0xC0800000

#define SHIFT(addr) (addr >> 12)
#define UNSHIFT(addr) (addr << 12)

namespace Memory
{
    namespace Pages
    {

        Directory kernel_directory;
        Directory *current_directory = 0;

        void init()
        {
            kernel_directory.physicalAddress = KERNEL_DIRECTORY_ADDRESS;
            kernel_directory.entries = (DirectoryEntry *)KERNEL_VIRTUAL_DIRECTORY_ADDRESS;
            kernel_directory.tables = (TableEntry *)KERNEL_VIRTUAL_TABLES_ADDRESS;

            current_directory = &kernel_directory;
        }

        void createTable(uint16 index, bool user)
        {
            ASSERT(index >= 0 && index < 1024);

            uint32 physicalAddress = Physical::get_free_pages(1);
            current_directory->entries[index].address = SHIFT(physicalAddress);
            current_directory->entries[index].write = 1;
            current_directory->entries[index].user = (uint32)user;
            mapPage(((uint32)current_directory->tables + index * 4096), physicalAddress, user);
            current_directory->entries[index].present = 1;
        }

        void mapPage(uint32 virtualAddress, uint32 physicalAddress, bool user)
        {
            ASSERT((virtualAddress & 0xFFFFF000) == virtualAddress);   // check if aligned
            ASSERT((physicalAddress & 0xFFFFF000) == physicalAddress); // check if aligned

            uint32 index = virtualAddress / 4096;

            if (current_directory->entries[index / 1024].present == 0) // check if table is present
                createTable(index / 1024, user);

            current_directory->tables[index].address = SHIFT(physicalAddress);
            current_directory->tables[index].write = 1;
            current_directory->tables[index].user = (uint32)user;
            current_directory->tables[index].present = 1;
        }

        void unmapPage(uint32 virtualAddress)
        {
            uint32 index = virtualAddress / 4096;
            ((uint32 *)current_directory->tables)[index] = 0;
        }

        uint32 getKernelFreePage()
        {
            int freeTableIndex = 0;
            for (int i = 770; i < 1024; i++)
            {
                if (current_directory->entries[i].present == 0)
                {
                    freeTableIndex = i;
                    continue;
                }

                for (int j = 0; j < 1024; j++)
                {
                    if (current_directory->tables[i * 1024 + j].present == 0)
                        return (i * 1024 + j) * 4096;
                }
            }

            if (freeTableIndex == 0)
                panic("Out of kernel free pages");

            createTable(freeTableIndex, false);
            return freeTableIndex * 1024 * 4096;
        }

        uint16 getKernelFreeTable()
        {
            for (int i = 770; i < 1024; i++)
            {
                if (!current_directory->entries[i].present)
                    return i;
            }

            panic("Out of kernel free tables");
            return 0; // not reached
        }

        void allocPages(uint32 count, uint32 virtualAddress)
        {
            for (uint32 i = 0; i < count; i++)
            {
                mapPage(virtualAddress, Physical::get_free_pages(1), true);
                virtualAddress += 4096;
            }
        }

        void switchDirectory(Directory *newDirectory)
        {
            current_directory = newDirectory;
            __asm__ volatile("mov %0, %%cr3" ::"r"(newDirectory->physicalAddress));
        }

        void createDirectory(Directory *directory)
        {
            // temp maping
            uint32 addressPhysical = Physical::get_free_pages(1);
            directory->physicalAddress = addressPhysical;
            uint32 addressVirtual = getKernelFreePage();
            mapPage(addressVirtual, addressPhysical, false);

            directory->entries = (DirectoryEntry *)addressVirtual;

            // temp maping
            uint32 tablePhysicalAddress = Physical::get_free_pages(1);
            uint32 tableVirtualAddress = getKernelFreePage();
            mapPage(tableVirtualAddress, tablePhysicalAddress, false);

            //recursive maping
            {
                directory->entries[767].address = SHIFT(tablePhysicalAddress);
                directory->entries[767].write = 1;
                directory->entries[767].present = 1;

                ((TableEntry *)tableVirtualAddress)[767].address = SHIFT(tablePhysicalAddress);
                ((TableEntry *)tableVirtualAddress)[767].write = 1;
                ((TableEntry *)tableVirtualAddress)[767].present = 1;
            }

            directory->tables = (TableEntry *)0xBFC00000;

            uint32 tableForDirectoryPhysicalAddress = Physical::get_free_pages(1);
            uint32 tableForDirectoryVirtualAddress = getKernelFreePage();
            mapPage(tableForDirectoryVirtualAddress, tableForDirectoryPhysicalAddress, false);

            directory->entries[766].address = SHIFT(tableForDirectoryPhysicalAddress);
            directory->entries[766].write = 1;
            directory->entries[766].user = 1;
            directory->entries[766].present = 1;

            //kernel maping
            for (int i = 768; i < 1024; i++)
            {
                if (kernel_directory.entries[i].present)
                {
                    directory->entries[i] = kernel_directory.entries[i];
                    ((TableEntry *)tableVirtualAddress)[i].address = kernel_directory.entries[i].address;
                    ((TableEntry *)tableVirtualAddress)[i].write = 1;
                    ((TableEntry *)tableVirtualAddress)[i].present = 1;
                }
            }

            ((TableEntry *)tableForDirectoryVirtualAddress)[1023].address = SHIFT(addressPhysical);
            ((TableEntry *)tableForDirectoryVirtualAddress)[1023].write = 1;
            ((TableEntry *)tableForDirectoryVirtualAddress)[1023].present = 1;

            ((TableEntry *)tableVirtualAddress)[766].address = SHIFT(tableForDirectoryPhysicalAddress);
            ((TableEntry *)tableVirtualAddress)[766].write = 1;
            ((TableEntry *)tableVirtualAddress)[766].present = 1;

            directory->entries = (DirectoryEntry *)0xBFBFF000;

            unmapPage(addressVirtual);
            unmapPage(tableVirtualAddress);
        }

    } // namespace Pages

} // namespace Memory