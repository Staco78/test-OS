#include "idt.h"

extern void keyboard_interrupt(void);

static
    __attribute__((aligned(0x1000)))
    idt_desc_t idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtp;

void idt_set_descriptor(uint8 vector, uint32 isr, uint8 flags)
{
    idt_desc_t *descriptor = &idt[vector];

    descriptor->base_low = isr & 0xFFFF;
    descriptor->cs = 0x08;
    descriptor->attributes = flags;
    descriptor->base_high = (isr >> 16) & 0xFFFF;
    descriptor->reserved = 0;
}


void idt_assemble()
{

    idtp.base = (uint32)&idt[0];
    idtp.limit = (uint16)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    for (uint8 vector = 0; vector < IDT_CPU_EXCEPTION_COUNT; vector++)
    {
        idt_set_descriptor(vector, (uint32)keyboard_interrupt, 0x8E);
    }

    idt_set_descriptor(0x21, (uint32)keyboard_interrupt, 0x8E);

    write_port(0x20, 0x11);
    write_port(0xA0, 0x11);

    /* ICW2 - remap offset address of IDT */
    /*
	* In x86 protected mode, we have to remap the PICs beyond 0x20 because
	* Intel have designated the first 32 interrupts as "reserved" for cpu exceptions
	*/
    write_port(0x21, 0x20);
    write_port(0xA1, 0x28);

    /* ICW3 - setup cascading */
    write_port(0x21, 0x00);
    write_port(0xA1, 0x00);

    /* ICW4 - environment info */
    write_port(0x21, 0x01);
    write_port(0xA1, 0x01);
    /* Initialization finished */

    /* mask interrupts */
    write_port(0x21, 0xff);
    write_port(0xA1, 0xff);

    __asm__ volatile("lidt %0"
                     :
                     : "m"(idtp));
    __asm__ volatile("sti");

    write_port(0x21, 0xFD);
}
