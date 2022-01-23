#pragma once
#include "types.h"

#define IDT_MAX_DESCRIPTORS 256
#define IDT_CPU_EXCEPTION_COUNT 32

#define IDT_DESCRIPTOR_X16_INTERRUPT 0x06
#define IDT_DESCRIPTOR_X16_TRAP 0x07
#define IDT_DESCRIPTOR_X32_TASK 0x05
#define IDT_DESCRIPTOR_X32_INTERRUPT 0x0E
#define IDT_DESCRIPTOR_X32_TRAP 0x0F
#define IDT_DESCRIPTOR_RING1 0x40
#define IDT_DESCRIPTOR_RING2 0x20
#define IDT_DESCRIPTOR_RING3 0x60
#define IDT_DESCRIPTOR_PRESENT 0x80

#define IDT_DESCRIPTOR_EXCEPTION (IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_EXTERNAL (IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT)
#define IDT_DESCRIPTOR_CALL (IDT_DESCRIPTOR_X32_INTERRUPT | IDT_DESCRIPTOR_PRESENT | IDT_DESCRIPTOR_RING3)

typedef struct
{
	uint16 base_low;
	uint16 cs;
	uint8 reserved;
	uint8 attributes;
	uint16 base_high;
} __attribute__((packed)) idt_desc_t;

typedef struct
{
	uint16 limit;
	uint32 base;
} __attribute__((packed)) idtr_t;

struct InterruptRegisters
{
	uint32 gs, fs, es, ds;
	uint32 edi, esi, sbp, esp, ebx, edx, ecx, eax;
	uint32 eip, cs, eflags, useresp, ss;
} __attribute__((packed));

void idt_assemble();