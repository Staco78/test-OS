#pragma once
#include "types.h"
#include "memory.h"

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
    uint32 eax;
    uint32 ebx;
    uint32 ecx;
    uint32 edx;
    uint32 esi;
    uint32 edi;

    uint32 esp;
    uint32 ebp;

    uint32 eip;

    uint32 eflags;
};

struct Process
{
    ProcessId id;
    Memory::Pages::Directory pagingDirectory;
    Registers regs;
};

void create_process(const char *name);