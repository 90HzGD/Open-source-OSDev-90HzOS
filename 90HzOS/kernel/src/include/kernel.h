#ifndef KERNEL_H
    #define KERNEL_H

    #define VRAM_CHAR_ADR           0xB8000
    #define VRAM_ATT_ADR            0xB8001
    #define VGA_SCREEN_WIDTH        80
    #define VGA_SCREEN_HEIGHT       25

    #ifdef INCLUDE_FROM_KRNL

        #define KB_COMMAND              0x64
        #define KB_DATA                 0x60

        #define BASE_PTR_INITRAM        0x8000
        #define LENGTH_INITRAM          0x9000

        struct avail_RAM {
            unsigned int* baseptr[64];
            unsigned int  length[64];
            unsigned int* heap_begin;
            unsigned int total_RAM;
        };

        struct avail_RAM initRAMstruct;

        int extended_key;

        enum Return_codes_main {
            OK = 0,
        };
    #endif
        
    void clear_screen(volatile unsigned int* position);
    void print_char(volatile const char displayed_char, const char attributes, volatile unsigned int *position);
    void print_string(volatile const char *string, const char attributes,volatile  unsigned int *position);
    void set_BGcolor(const char color);
    void replace_string(volatile char *string, volatile char *target);
    enum Return_codes_main init_RAM();

    unsigned char handle_kb();

#endif