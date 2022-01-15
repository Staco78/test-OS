#pragma once
#include "types.h"
#include "memory.h"

struct tss_entry_struct
{
    uint32 prev_tss; // The previous TSS - with hardware task switching these form a kind of backward linked list.
    uint32 esp0;     // The stack pointer to load when changing to kernel mode.
    uint32 ss0;      // The stack segment to load when changing to kernel mode.
    // Everything below here is unused.
    uint32 esp1; // esp and ss 1 and 2 would be used when switching to rings 1 or 2.
    uint32 ss1;
    uint32 esp2;
    uint32 ss2;
    uint32 cr3;
    uint32 eip;
    uint32 eflags;
    uint32 eax;
    uint32 ecx;
    uint32 edx;
    uint32 ebx;
    uint32 esp;
    uint32 ebp;
    uint32 esi;
    uint32 edi;
    uint32 es;
    uint32 cs;
    uint32 ss;
    uint32 ds;
    uint32 fs;
    uint32 gs;
    uint32 ldt;
    uint16 trap;
    uint16 iomap_base;
} __attribute__((packed));

void write_tss(uint32 gdtAddress)
{
    tss_entry_struct *tss_entry = (tss_entry_struct *)Memory::KernelAlloc::kmalloc(sizeof(tss_entry_struct));
    uint8 *gdt = (uint8 *)gdtAddress + 8 * 5;

    uint32 base = (uint32)tss_entry;

    ((uint16 *)gdt)[0] = sizeof(tss_entry_struct);
    ((uint16 *)gdt)[1] = base;
    gdt[4] = (uint8)(base >> 16);
    gdt[5] = 0b10001001;
    gdt[6] = 0b00000000;
    gdt[7] = (uint8)(base >> 24);

    for (int i = 0; i < sizeof(tss_entry_struct); i++)
    {
        ((uint8 *)tss_entry)[i] = 0;
    }

    tss_entry->ss0 = 0x10;
    tss_entry->esp0 = 0xC0090000;
    tss_entry->cr3 = 0xC0100000;

    tss_entry->cs = 0x08 | 3;
    tss_entry->ss = tss_entry->ds = tss_entry->es = tss_entry->fs = tss_entry->gs = (8 * 2) | 3;

    __asm__ volatile("mov $(5 * 8), %ax\n"
                     "ltr %ax");
}