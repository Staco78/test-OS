#include "processes.h"
#include "memory.h"
#include "fs.h"
#include "terminal.h"
#include "lib/memoryUtils.h"

void load_process(Process *process, const char *path)
{
    Fs::DirectoryEntry file;
    Fs::findEntry(&file, path);
    Fs::Inode fileInode = Fs::readInode(file.inode);

    uint8 *buff = (uint8 *)Memory::KernelAlloc::kmalloc(4096);
    Fs::readInodeData(&fileInode, buff, 0, 4096);
    ElfHeader header;
    memcpy(&header, buff, sizeof(ElfHeader));

    // print("magic: ");
    // printHex(header.magic);
    // print(" bits: ");
    // printInt(header.bits);
    // print(" endian: ");
    // printInt(header.endian);
    // print("\nheader version: ");
    // printInt(header.elfHeaderVersion);
    // print(" OS ABI: ");
    // printInt(header.OS_ABI);
    // print(" type: ");
    // printInt(header.type);
    // print("\narch: ");
    // printHex(header.architecture);
    // print(" version: ");
    // printInt(header.elfVersion);
    // print(" entry point: ");
    // printHex(header.programEntry);
    // print("\nheader table pos: ");
    // printHex(header.programHeaderTablePosition);
    // print(" section table pos: ");
    // printHex(header.sectionHeaderPosition);
    // print("\nflags: ");
    // printHex(header.flags);
    // print(" header size: ");
    // printHex(header.headerSize);
    // print(" program header entry size: ");
    // printHex(header.programHeaderEntrySize);
    // print("\nprogram header entries: ");
    // printHex(header.programHeaderEntriesNb);
    // print(" section header entry size: ");
    // printHex(header.sectionHeaderEntrySize);
    // print("\nsection header entries: ");
    // printHex(header.sectionHeaderEntrySize);
    // print(" section header index: ");
    // printInt(header.sectionHeaderIndex);

    ElfProgramHeader *programs = (ElfProgramHeader *)Memory::KernelAlloc::kmalloc(sizeof(ElfProgramHeader) * header.programHeaderEntriesNb);
    memcpy(programs, buff + header.programHeaderTablePosition, sizeof(ElfProgramHeader) * header.programHeaderEntriesNb);
    for (int i = 0; i < header.programHeaderEntriesNb; i++)
    {
        if (programs[i].type != (uint32)ElfSegmentType::load)
            continue;

        Memory::Pages::alloc_pages(&process->pagingDirectory, programs[i].sizeInMem / 4096 + (programs[i].sizeInMem % 4096 == 0 ? 0 : 1), programs[i].virtualAddress);
        Fs::readInodeData(&fileInode, (void *)programs[i].virtualAddress, programs[i].offset, programs[i].sizeInFile);

        // TODO: fill padding with 0
    }

    process->regs.eip = header.programEntry;

    Memory::KernelAlloc::kfree(buff);
}

extern "C" void jump_userMode(uint32 addr, uint32 esp, uint32 ebp);

void create_process(const char *path)
{

    Process *process = (Process *)Memory::KernelAlloc::kmalloc(sizeof(Process));
    process->id = 0;
    Memory::Pages::create_directory(&process->pagingDirectory);
    Memory::Pages::copy_kernel_pages(&process->pagingDirectory);

    __asm__("mov %0, %%cr3" ::"r"(process->pagingDirectory.physicalAddress));

    load_process(process, path);

    Memory::Pages::alloc_pages(&process->pagingDirectory, 1, 0x501000);
    process->regs.esp = 0x501000;
    process->regs.ebp = 0x502000;

    jump_userMode(process->regs.eip, process->regs.esp, process->regs.ebp);
}