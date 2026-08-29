#include "../include/vga/stdio.h"
#include "../include/kernel.h"
#include "../include/string.h"
    void change_color(const char color, volatile unsigned int *position){
        *((unsigned char*)VRAM_ATT_ADR+(*position*2)) = color;
        return;
    }
    
    void clear_screen(){
        extern volatile unsigned int position;
        extern volatile unsigned int Times_Grid_moved;
        for (unsigned int i=0; i<(VGA_SCREEN_WIDTH*VGA_SCREEN_HEIGHT); ++i){
            *((unsigned char*)VRAM_CHAR_ADR+(i*2)) = 0;
            *((unsigned char*)VRAM_ATT_ADR+(i*2)) = 0x0F;
        }
        position = 0;
        Times_Grid_moved = 0;
        return;
    }

    void move_grid(unsigned int count){
        if (count < 80){
            for (unsigned int i=0; i!=count; ++i){
                for (unsigned int j=0; j!=80*25; ++j){
                    *((unsigned char*)VRAM_CHAR_ADR+(j*2)) = *((unsigned char*)MOVE_GRID_BEGIN_CHAR+(j*2));
                    *((unsigned char*)VRAM_ATT_ADR+(j*2)) = *((unsigned char*)MOVE_GRID_BEGIN_ATT+(j*2));
                }
            }
            for (unsigned int i=0; i!=25; ++i){
                *((unsigned char*)MOVE_GRID_END+(i*2)) = 0;
                *((unsigned char*)MOVE_GRID_END+1+(i*2)) = 0;
            }
        }
        else {
            clear_screen();
        }
    }

    void print_char(volatile const unsigned char displayed_char, const char attributes, volatile unsigned int *position){
        extern volatile unsigned int Times_Grid_moved;
        if (*position-(Times_Grid_moved*80) >= (80*25-1)){
            move_grid(1);
            Times_Grid_moved += 1;
        }
        if (displayed_char != '\n' && displayed_char != '\t'){
            *((unsigned char*)VRAM_CHAR_ADR+((*position-(Times_Grid_moved*80))*2)) = displayed_char;
            *((unsigned char*)VRAM_ATT_ADR+((*position-(Times_Grid_moved*80))*2)) = attributes;
            ++(*position);
            return;
        }
        else if (displayed_char == '\n'){
            *(position) += 80 - (*position % 80);
            return;
        }
        else if(displayed_char == '\t'){
            unsigned int spaces= 4-(*position % 4);
            for (unsigned int i=0; i<spaces; ++i){
                *((unsigned char*)VRAM_CHAR_ADR+(*position*2)) = ' ';
                *((unsigned char*)VRAM_ATT_ADR+(*position*2)) = attributes;
                ++(*position);
            }
            return;
        }
    }

    void print_string(volatile const char* string, const char attributes, volatile unsigned int* position){
        for (unsigned int i=0; *(string+i)!='\0'; ++i){
            print_char(*(string+i), attributes, position);
        }
    }

    void set_BGcolor(const char color){
        for (unsigned int i=0; i<(VGA_SCREEN_WIDTH*VGA_SCREEN_HEIGHT); ++i){
            *((unsigned char*)VRAM_ATT_ADR+(i*2)) = color;
        }
    }

    void printf(const char* string, ...){
        extern volatile unsigned int position;
        extern volatile unsigned char Color;
        int** var_arg_ptr_ptr = (int**)(&string);       // very inspired name tho
        int* var_arg_ptr = (int*)(&string);             // 4 bytes per increase (stack aligned)
        for (int i = 0; *(string + i) != 0; ++i){
            if (*(string + i) != '%' && *(string + i) != '\033'){
                print_char(*(string + i), Color, &position);
            }
            else if(*(string + i) == '\033'){
                ++i;
                switch (*(string + i)){
                    case 'c':
                        clear_screen();
                        break;
                    default:
                        Color = *(string + i);
                        break;
                }
                continue;
            }
            else {
                ++var_arg_ptr;
                ++var_arg_ptr_ptr;
                ++i;
                switch (*(string + i)){
                    case 'd':case 'i':
                        print_integer(*var_arg_ptr, &position);
                        break;
                    case 'u':
                        print_uinteger(*var_arg_ptr, &position);
                        break;
                    case 'c':
                        print_char(*var_arg_ptr, Color, &position);
                        break;
                    case 's':
                        char stringf[8192];
                        unsigned int str_idx = 0;
                        unsigned int charf;
                        charf = **(var_arg_ptr_ptr);
                        while (charf != 0 && str_idx < 8192){
                            charf = **(var_arg_ptr_ptr);
                            *(stringf + str_idx) = charf;
                            ++*(var_arg_ptr);
                            ++str_idx;
                        }
                        *(stringf + str_idx) = 0;
                        print_string(stringf, Color, &position); 
                        break;
                    case 'p':case 'h':
                        const unsigned int* int_ptr = (unsigned int*)*(var_arg_ptr);
                        print_hex(int_ptr, &position);
                        break;
                    default:
                        --i;
                        print_char('%', Color, &position);
                        continue;
                }
                continue;
            }
        }
    }

    void print_integer(int integer, volatile unsigned int* position){
        extern volatile unsigned char Color;
        char int_string[11];
        override_str(int_string, 11);
        char unit;
        unsigned char int_to_char = 0;
        unsigned int integer_len = 0;

        if (integer < 0){
            print_char('-', Color, position);
            integer *= -1;
        }
        if (integer != 0 && integer >= 10) {

            int integer_copy = integer;

            for (unsigned int i = 0; integer_copy >= 10; ++i){
                integer_copy /= 10;
                integer_len++;
            }
            *(int_string + integer_len) = 0;

            for (unsigned int i = 0; integer >= 1; ++i){
                unit = integer % 10;
                integer /= 10;

                switch (unit){
                    case 0:
                        int_to_char = 48;
                        break;
                    case 1:
                        int_to_char = 49;
                        break;
                    case 2:
                        int_to_char = 50;
                        break;
                    case 3:
                        int_to_char = 51;
                        break;
                    case 4:
                        int_to_char = 52;
                        break;
                    case 5:
                        int_to_char = 53;
                        break;
                    case 6:
                        int_to_char = 54;
                        break;
                    case 7:
                        int_to_char = 55;
                        break;
                    case 8:
                        int_to_char = 56;
                        break;
                    case 9:
                        int_to_char = 57;
                        break;
                    default:
                        int_to_char = 0;
                        break;
                }
                *(int_string + integer_len - i) = int_to_char;
            }
        }
        else if (integer < 10){
            switch (integer){
                case 0:
                    int_to_char = 48;
                    break;
                case 1:
                    int_to_char = 49;
                    break;
                case 2:
                    int_to_char = 50;
                    break;
                case 3:
                    int_to_char = 51;
                    break;
                case 4:
                    int_to_char = 52;
                    break;
                case 5:
                    int_to_char = 53;
                    break;
                case 6:
                    int_to_char = 54;
                    break;
                case 7:
                    int_to_char = 55;
                    break;
                case 8:
                    int_to_char = 56;
                    break;
                case 9:
                    int_to_char = 57;
                    break;
                default:
                    int_to_char = 0;
                    break;
            }
            *(int_string) = int_to_char;
            *(int_string + 1) = 0;
        }
        else {
            *(int_string) = 48;
            *(int_string + 1) = 0;
        }
        print_string(int_string, Color, position);
    }

    void print_uinteger(unsigned int uinteger, volatile unsigned int* position){
        extern volatile unsigned char Color;
        char uint_string[11];
        override_str(uint_string, 11);
        unsigned char uint_to_char = 0;
        unsigned char unit;
        unsigned int  uinteger_len = 0;

        unsigned int uinteger_copy = uinteger;

        if (uinteger != 0 && uinteger >= 10){

            for (int i = 0; uinteger_copy >= 10; ++i){
                uinteger_copy /= 10;
                uinteger_len++;
            }
            *(uint_string + uinteger_len) = 0;

            for (unsigned int i = 0; uinteger >= 1; ++i){
                unit = uinteger % 10;

                uinteger /= 10;

                switch (unit){
                    case 0:
                        uint_to_char = 48;
                        break;
                    case 1:
                        uint_to_char = 49;
                        break;
                    case 2:
                        uint_to_char = 50;
                        break;
                    case 3:
                        uint_to_char = 51;
                        break;
                    case 4:
                        uint_to_char = 52;
                        break;
                    case 5:
                        uint_to_char = 53;
                        break;
                    case 6:
                        uint_to_char = 54;
                        break;
                    case 7:
                        uint_to_char = 55;
                        break;
                    case 8:
                        uint_to_char = 56;
                        break;
                    case 9:
                        uint_to_char = 57;
                        break;
                    default:
                        uint_to_char = 0;
                        break;
                }
                *(uint_string + uinteger_len - i) = uint_to_char;
            }
        }
        else if(uinteger < 10){
            switch (uinteger){
                case 0:
                    uint_to_char = 48;
                    break;
                case 1:
                    uint_to_char = 49;
                    break;
                case 2:
                    uint_to_char = 50;
                    break;
                case 3:
                    uint_to_char = 51;
                    break;
                case 4:
                    uint_to_char = 52;
                    break;
                case 5:
                    uint_to_char = 53;
                    break;
                case 6:
                    uint_to_char = 54;
                    break;
                case 7:
                    uint_to_char = 55;
                    break;
                case 8:
                    uint_to_char = 56;
                    break;
                case 9:
                    uint_to_char = 57;
                    break;
                default:
                    uint_to_char = 0;
                    break;
                }
            *(uint_string) = uint_to_char;
            *(uint_string + 1) = 0;

        }
        else {
            *(uint_string) = 48;
            *(uint_string + 1) = 0;
        }
        print_string(uint_string, Color, position);
    }

    void print_hex(const unsigned int* ptr, volatile unsigned int* position){
        unsigned int conv_ptr = (unsigned int)ptr;
        extern volatile unsigned char Color;
        char ptr_string[11];
        unsigned char uptr_to_char = 0;
        unsigned char unithex;
        unsigned int hex_len = 0;
        unsigned int conv_ptr_copy = conv_ptr;

        print_string("0x", Color, position);

        if (ptr != 0){

            for (unsigned int i = 0; conv_ptr_copy >= 16; ++i){
                conv_ptr_copy /= 16;
                ++hex_len;
            }

            *(ptr_string + 8) = 0;
            unsigned char offset = 0;
            
            for (unsigned int i = 0; i != (8-hex_len); ++i){
                *(ptr_string + i) = 48;
                offset += 1;
            }
            unsigned int index = 0;

            for (unsigned int i = 0; conv_ptr >= 1; ++i){
                unithex = conv_ptr % 16;
                conv_ptr /= 16;

                switch (unithex){
                    case 0:
                        uptr_to_char = 48;
                        break;
                    case 1:
                        uptr_to_char = 49;
                        break;
                    case 2:
                        uptr_to_char = 50;
                        break;
                    case 3:
                        uptr_to_char = 51;
                        break;
                    case 4:
                        uptr_to_char = 52;
                        break;
                    case 5:
                        uptr_to_char = 53;
                        break;
                    case 6:
                        uptr_to_char = 54;
                        break;
                    case 7:
                        uptr_to_char = 55;
                        break;
                    case 8:
                        uptr_to_char = 56;
                        break;
                    case 9:
                        uptr_to_char = 57;
                        break;
                    case 0x0A:
                        uptr_to_char = 65;
                        break;
                    case 0x0B:
                        uptr_to_char = 66;
                        break;
                    case 0x0C:
                        uptr_to_char = 67;
                        break;
                    case 0x0D:
                        uptr_to_char = 68;
                        break;
                    case 0x0E:
                        uptr_to_char = 69;
                        break;
                    case 0x0F:
                        uptr_to_char = 70;
                        break;
                    default:
                        uptr_to_char = 0;
                        break;
                ++index;
                }
                *(ptr_string + hex_len - i + offset - 1) = uptr_to_char;
            }
        }
        else {
            for (unsigned int i = 0; i != 8; ++i){
                *(ptr_string + i) = 48;
                *(ptr_string + i + 1) = 0;
            }
        }
        print_string(ptr_string, Color, position);

    }