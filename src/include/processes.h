#pragma once
#include "types.h"
#include "memory.h"
#include "idt.h"

struct ElfHeader
{
    uint32 magic;
    uint8 bits;   // 1 = 32 bit, 2 = 64 bit
    uint8 endian; // 1 = little endian, 2 = big endian
    uint8 elfHeaderVersion;
    uint8 OS_ABI;
    uint32 padding1;
    uint32 padding2;
    uint16 type; // 1 = relocatable, 2 = executable, 3 = shared, 4 = core
    uint16 architecture;
    uint32 elfVersion;
    uint32 programEntry;
    uint32 programHeaderTablePosition;
    uint32 sectionHeaderPosition;
    uint32 flags;
    uint16 headerSize;
    uint16 programHeaderEntrySize;
    uint16 programHeaderEntriesNb;
    uint16 sectionHeaderEntrySize;
    uint16 sectionHeaderEntriesNb;
    uint16 sectionHeaderIndex;
} __attribute__((packed));

enum class Architecture
{
    Sparc = 2,
    x86 = 3,
    MIPS = 8,
    PowerPC = 0x14,
    ARM = 0x28,
    SuperH = 0x2A,
    IA64 = 0x32,
    x86_64 = 0x3E,
    AArch64 = 0xB7,
    RISC_V = 0xF3
};

struct ElfProgramHeader
{
    uint32 type;
    uint32 offset;
    uint32 virtualAddress;
    uint32 unused;
    uint32 sizeInFile;
    uint32 sizeInMem;
    uint32 flags;
    uint32 alignement;
} __attribute__((packed));

enum class ElfSegmentType
{
    null = 0,
    load = 1,
    dynamic = 2,
    interp = 3
};

typedef uint32 ProcessId;

struct Registers
{
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32 eip, cs, eflags, useresp, ss;
} __attribute__((packed));

struct Process
{
    ProcessId id;
    Memory::Pages::Directory pagingDirectory;
    Registers regs;
    bool started = false;
    uint32 entry;
};

void create_process(const char *name);

class ProcessesQueue
{
public:
    ProcessesQueue();
    Process *pop();
    Process *first();
    void push(Process *process);
    uint32 getSize();

private:
    uint32 m_size = 0;
    uint32 m_data;
    uint32 m_maxData;
    uint32 m_pushIndex;
    uint32 m_popIndex;
};

namespace Scheduler
{
    void init();
    void schedule(Registers *regs);
    void add(Process *process);
    void setCurrent(Process *process);
} // namespace Scheduler
