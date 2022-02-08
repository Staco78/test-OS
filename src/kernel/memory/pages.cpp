#include "memory.h"
#include "panic.h"
#include "terminal.h"
#include "debug.h"
#include "lib/memoryUtils.h"

#define DIRECTORY_ADDRESS 0x100000
#define VIRTUAL_DIRECTORY_ADDRESS 0xC0800000
#define VIRTUAL_TABLES_ADDRESS 0xC0400000

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
            kernel_directory.physicalAddress = DIRECTORY_ADDRESS;
            kernel_directory.entries = (DirectoryEntry *)VIRTUAL_DIRECTORY_ADDRESS;
            kernel_directory.tables = (TableEntry *)VIRTUAL_TABLES_ADDRESS;

            current_directory = &kernel_directory;
        }

        void createTable(uint16 index, uint32 flags)
        {
            ASSERT(index >= 0 && index < 1024);

            uint32 physicalAddress = Physical::get_free_pages(1);
            current_directory->entries[index].address = SHIFT(physicalAddress);
            ((uint32 *)current_directory->entries)[index] |= flags;
            mapPage(((uint32)current_directory->tables + index * 4096), physicalAddress, (flags | (uint32)Flags::write) & ~(uint32)Flags::user);
            current_directory->entries[index].present = 1;
        }

        void mapPage(uint32 virtualAddress, uint32 physicalAddress, uint32 flags)
        {
            ASSERT((virtualAddress & 0xFFFFF000) == virtualAddress);   // check if aligned
            ASSERT((physicalAddress & 0xFFFFF000) == physicalAddress); // check if aligned

            uint32 index = virtualAddress / 4096;

            if (current_directory->entries[index / 1024].present == 0) // check if table is present
                createTable(index / 1024, flags);

            current_directory->tables[index].address = SHIFT(physicalAddress);
            ((uint32 *)current_directory->tables)[index] |= flags;
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

        void allocPages(uint32 count, uint32 virtualAddress, uint32 flags)
        {
            for (uint32 i = 0; i < count; i++)
            {
                mapPage(virtualAddress, Physical::get_free_pages(1), flags);
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

            //recursive maping
            {
                directory->entries[769].address = SHIFT(tablePhysicalAddress);
                directory->entries[769].write = 1;
                directory->entries[769].present = 1;

                ((TableEntry *)tableVirtualAddress)[769].address = SHIFT(tablePhysicalAddress);
                ((TableEntry *)tableVirtualAddress)[769].write = 1;
                ((TableEntry *)tableVirtualAddress)[769].present = 1;
            }

            directory->tables = (TableEntry *)VIRTUAL_TABLES_ADDRESS;

            uint32 tableForDirectoryPhysicalAddress = Physical::get_free_pages(1);
            uint32 tableForDirectoryVirtualAddress = getKernelFreePage();
            mapPage(tableForDirectoryVirtualAddress, tableForDirectoryPhysicalAddress, false);

            directory->entries[770].address = SHIFT(tableForDirectoryPhysicalAddress);
            directory->entries[770].write = 1;
            directory->entries[770].user = 1;
            directory->entries[770].present = 1;

            memcpy((void *)tableForDirectoryVirtualAddress, &kernel_directory.tables[1024 * 770], 4096);

            ((TableEntry *)tableForDirectoryVirtualAddress)[0].address = SHIFT(addressPhysical);
            ((TableEntry *)tableForDirectoryVirtualAddress)[0].write = 1;
            ((TableEntry *)tableForDirectoryVirtualAddress)[0].present = 1;

            ((TableEntry *)tableVirtualAddress)[770].address = SHIFT(tableForDirectoryPhysicalAddress);
            ((TableEntry *)tableVirtualAddress)[770].write = 1;
            ((TableEntry *)tableVirtualAddress)[770].present = 1;

            directory->entries = (DirectoryEntry *)VIRTUAL_DIRECTORY_ADDRESS;

            unmapPage(addressVirtual);
            unmapPage(tableVirtualAddress);
            unmapPage(tableForDirectoryVirtualAddress);
        }

        bool isUserPtrValid(uint32 ptr, uint32 size, bool write)
        {
            if (ptr == 0)
                return false;
            if (ptr >= 0xC0000000)
                return false;

            uint32 end = ptr ? ptr + (size - 1) : ptr;

            if (end >= 0xC0000000)
                return false;

            uint32 pageStart = SHIFT(ptr);
            uint32 pageEnd = SHIFT(end);

            for (uint32 page = pageStart; page <= pageEnd; page++)
            {
                uint16 table = page / 1024;
                if (!current_directory->entries[table].present)
                    return false;
                if (!current_directory->entries[table].user)
                    return false;
                if (write && !current_directory->entries[table].write)
                    return false;

                if (!current_directory->tables[page].present)
                    return false;
                if (!current_directory->tables[page].user)
                    return false;
                if (write && !current_directory->tables[page].write)
                    return false;
            }
            return true;
        }

    } // namespace Pages

} // namespace Memory