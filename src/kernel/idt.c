#include "idt.h"

#include "exceptions.h"

extern void keyboard_interrupt(void);

static
    __attribute__((aligned(0x1000)))
    idt_desc_t idt[IDT_MAX_DESCRIPTORS];

static idtr_t idtp;

void exception_handler()
{
    panic("Exception");
}

void idt_set_descriptor(uint8 vector, uint32 isr)
{
    idt_desc_t *descriptor = &idt[vector];

    descriptor->base_low = isr & 0xFFFF;
    descriptor->cs = 0x08;
    descriptor->attributes = 0x8E;
    descriptor->base_high = (isr >> 16) & 0xFFFF;
    descriptor->reserved = 0;
}

void idt_assemble()
{

    idtp.base = (uint32)&idt[0];
    idtp.limit = (uint32)sizeof(idt_desc_t) * IDT_MAX_DESCRIPTORS - 1;

    idt_set_descriptor(0, (uint32)_0);
    idt_set_descriptor(1, (uint32)_1);
    idt_set_descriptor(2, (uint32)_2);
    idt_set_descriptor(3, (uint32)_3);
    idt_set_descriptor(4, (uint32)_4);
    idt_set_descriptor(5, (uint32)_5);
    idt_set_descriptor(6, (uint32)_6);
    idt_set_descriptor(7, (uint32)_7);
    idt_set_descriptor(8, (uint32)_8);
    idt_set_descriptor(9, (uint32)_9);
    idt_set_descriptor(10, (uint32)_10);
    idt_set_descriptor(11, (uint32)_11);
    idt_set_descriptor(12, (uint32)_12);
    idt_set_descriptor(13, (uint32)_13);
    idt_set_descriptor(14, (uint32)_14);

    idt_set_descriptor(15, (uint32)exception_handler);
    idt_set_descriptor(16, (uint32)exception_handler);
    idt_set_descriptor(17, (uint32)exception_handler);
    idt_set_descriptor(18, (uint32)exception_handler);
    idt_set_descriptor(19, (uint32)exception_handler);
    idt_set_descriptor(20, (uint32)exception_handler);
    idt_set_descriptor(21, (uint32)exception_handler);
    idt_set_descriptor(22, (uint32)exception_handler);
    idt_set_descriptor(23, (uint32)exception_handler);
    idt_set_descriptor(24, (uint32)exception_handler);
    idt_set_descriptor(25, (uint32)exception_handler);
    idt_set_descriptor(26, (uint32)exception_handler);
    idt_set_descriptor(27, (uint32)exception_handler);
    idt_set_descriptor(28, (uint32)exception_handler);
    idt_set_descriptor(29, (uint32)exception_handler);
    idt_set_descriptor(30, (uint32)exception_handler);
    idt_set_descriptor(31, (uint32)exception_handler);

    idt_set_descriptor(0x21, (uint32)keyboard_interrupt);

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
