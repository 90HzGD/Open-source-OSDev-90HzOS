#ifndef PORTS_H
    #define PORTS_H
    #define PIC1            0x20
    #define PIC2            0xA0
    #define PIC1_COMMAND    PIC1
    #define PIC1_DATA       (PIC1+1)
    #define PIC2_COMMAND    PIC2
    #define PIC2_DATA       (PIC2+1)
    #define PIC_EOI         0x20        // End of Interrupt

    extern unsigned char inb(unsigned short port);     // Short cz 2^16 ports
    extern unsigned short inw(unsigned short port);    
    extern unsigned int inl(unsigned short port);     

    extern void outb(unsigned short port, unsigned char value);

    extern void outw(unsigned short port, unsigned short value);

    extern void outl(unsigned short port, unsigned int value);
    
#endif