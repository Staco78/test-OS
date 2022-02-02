#include "processes.h"
#include "memory.h"
#include "fs.h"
#include "terminal.h"
#include "lib/memoryUtils.h"
#include "debug.h"

void load_process(Process *process, const char *path)
{
    Fs::DirectoryEntry file;
    Fs::findEntry(&file, path);
    Fs::Inode fileInode = Fs::readInode(file.inode);

    uint8 *buff = (uint8 *)Memory::KernelAlloc::kmalloc(4096);
    Fs::readInodeData(&fileInode, buff, 0, 4096);
    ElfHeader header;
    memcpy(&header, buff, sizeof(ElfHeader));

    ElfProgramHeader *programs = (ElfProgramHeader *)Memory::KernelAlloc::kmalloc(sizeof(ElfProgramHeader) * header.programHeaderEntriesNb);
    memcpy(programs, buff + header.programHeaderTablePosition, sizeof(ElfProgramHeader) * header.programHeaderEntriesNb);
    for (int i = 0; i < header.programHeaderEntriesNb; i++)
    {
        if (programs[i].type != (uint32)ElfSegmentType::load)
            continue;

        Memory::Pages::allocPages(programs[i].sizeInMem / 4096 + (programs[i].sizeInMem % 4096 == 0 ? 0 : 1), programs[i].virtualAddress, (uint32)Memory::Pages::Flags::user_write);
        Fs::readInodeData(&fileInode, (void *)programs[i].virtualAddress, programs[i].offset, programs[i].sizeInFile);

        // TODO: fill padding with 0
    }

    process->entry = header.programEntry;

    Memory::KernelAlloc::kfree(buff);
}

void create_process(const char *path)
{
    Process *process = (Process *)Memory::KernelAlloc::kmalloc(sizeof(Process));
    memset(process, 0, sizeof(Process));
    process->id = 0;

    Memory::Pages::createDirectory(&process->pagingDirectory);
    Memory::Pages::switchDirectory(&process->pagingDirectory);

    load_process(process, path);

    Memory::Pages::allocPages(1, 0x501000, (uint32)Memory::Pages::Flags::user_write);
    process->regs.esp = 0x501000;
    process->regs.ebp = 0x502000;
    process->regs.useresp = 0x501000;
    process->regs.eflags = 0x200;

    Scheduler::add(process);
}