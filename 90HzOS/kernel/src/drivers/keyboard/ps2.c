#include "../../include/drivers/keyboard/ps2.h"
#include "../../include/drivers/ports/ports.h"

struct IDT_entry IDT[256];        // 256 total ints

void init_idt(){
    IDT[0x21].segment = 8;              // 0x21 for keyboard, 2nd int
    IDT[0x21].zero = 0;
    IDT[0x21].type = 0b10001110;
    IDT[0x21].offset_lower = (unsigned int)kb_handler&0xFFFF;
    IDT[0x21].offset_upper = ((unsigned int) kb_handler&0xFFFF0000) >> 16;

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);

    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);

    outb(PIC1_DATA, 0x1);
    outb(PIC2_DATA, 0x1);

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);

    struct IDT_pointer idt_ptr;
    idt_ptr.offset = (unsigned int)&IDT;
    idt_ptr.limit = sizeof(struct IDT_entry)*256;
    load_idt(&idt_ptr);

}

void kb_init(){
    outb(PIC1_DATA, 0b11111101);
}