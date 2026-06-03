#ifndef MEM_ALLOCH

    #define MEM_ALLOCH

    unsigned int* malloc(unsigned int size);
    void free(unsigned int* ALLOC_ADR);
    unsigned int get_remain_heap_RAM(unsigned char mode);
    unsigned int* get_previous_bloc(unsigned int* bloc);
    unsigned int* get_next_bloc(unsigned int* bolc);
    void init_bloc(unsigned int* ALLOC_ADR);                // Overrides bloc content w/ only zeros

    unsigned int* malloc(unsigned int size){
        // Invalid Size
        if (size == 0){
            return 0x0;
        }
        // Makes size % 4
        size += 4 - (size % 4);
        size /= 4;
        unsigned int ALLOC_IDX = 0;
        unsigned int size_offset = 0;
        unsigned int* ret = 0;
        unsigned char status;
        unsigned int free_size;
        do {
            if (*(HEAP_START + ALLOC_IDX + size_offset) != 1 && *(HEAP_START + ALLOC_IDX + size_offset + 1) >= size * 4){
                status = *(HEAP_START + ALLOC_IDX + size_offset);
                free_size = *(HEAP_START + ALLOC_IDX + size_offset + 1);
                *(HEAP_START + ALLOC_IDX + size_offset) = 1;
                *(HEAP_START + ALLOC_IDX + size_offset + 1) = size * 4;
                ret = (HEAP_START + ALLOC_IDX + size_offset + 2);
                size_offset += size;

                if (status == 0){
                    *(HEAP_START + ALLOC_IDX + size_offset + 1) = 0;
                    if (*(&status + (free_size / 4)+1) == 0){
                        *(HEAP_START + ALLOC_IDX + size_offset + 2) = *(&status + (free_size / 4)+1) + size*4;
                    }
                }
                else {
                    *(HEAP_START + ALLOC_IDX + size_offset + 2) = 2;
                    unsigned int remain_RAM = get_remain_heap_RAM(0);
                    *(HEAP_START + ALLOC_IDX + size_offset + 3) = remain_RAM;
                }
                break;
        
            }
            size_offset += *(HEAP_START + ALLOC_IDX + size_offset + 1)/4;
            ++ALLOC_IDX;

        } while (ret == 0);
        return ret;
    }

    void free(unsigned int* ALLOC_ADR){
        if (ALLOC_ADR - 2 == initRAMstruct.heap_begin){
            return;
        }
        enum boolean last = *(ALLOC_ADR + ((*(ALLOC_ADR - 1))/4)) == 2;
        extern void freeze();
        unsigned int* prev_adr;
        unsigned int* size_ptr;
        extern void freeze();
        if (last){
            prev_adr = get_previous_bloc(ALLOC_ADR);
            if (*(prev_adr - 2) == 0){
                *(prev_adr - 2) = 2;
                *(prev_adr - 1) = get_remain_heap_RAM(0);
            }
            else {
                *(ALLOC_ADR - 2) = 2;
                *(ALLOC_ADR - 1) = get_remain_heap_RAM(0);
            }
            return;
        }
        else {
            prev_adr = get_previous_bloc(ALLOC_ADR);
            size_ptr = ALLOC_ADR - 1;
            if (*(prev_adr - 2) == 0){
                size_ptr = prev_adr - 1;
                *(size_ptr) += *(ALLOC_ADR - 1)+1;
            }
            else {
                *(ALLOC_ADR - 2) = 0;
            }
            unsigned int* next_adr = get_next_bloc(ALLOC_ADR);
            if (*(next_adr - 2) == 0){
                *(size_ptr) += *(next_adr - 1);
            }
            return;
        }
    }

    void write_string(unsigned char* string, unsigned int* STR_TARGET){
        for (unsigned int i = 0; *string != 0; ++i){
            *((unsigned char*)STR_TARGET + i) = *string;
            ++string;
        }
        return;
    }

    unsigned int get_remain_heap_RAM(unsigned char mode){
        unsigned char status = *(HEAP_START);
        unsigned int size_offset = 0;
        unsigned int size_offset_unused = 0;
        unsigned int ALLOC_IDX = 0;
        for (unsigned int i = 0; status != 2; ++i){
            status = *(HEAP_START + ALLOC_IDX + size_offset);
            size_offset += *(HEAP_START + ALLOC_IDX + size_offset + 1)/4;
            if (status == 0){
                size_offset_unused += *(HEAP_START + ALLOC_IDX + size_offset + 1);
            }
            ALLOC_IDX += 1;
        }
        unsigned int remain_RAM = initRAMstruct.total_RAM;
        remain_RAM -= (remain_RAM % 4);
        if (mode == 0){
            remain_RAM -= size_offset * 4 - ALLOC_IDX+1 - (unsigned int)HEAP_START;
        }
        else if (mode == 1){
            remain_RAM -= (size_offset * 4 - ALLOC_IDX+1 - (unsigned int)HEAP_START) + size_offset_unused;
        }
        remain_RAM -= (remain_RAM % 4);
        return remain_RAM;
    }

    unsigned int* get_previous_bloc(unsigned int* bloc){
        unsigned int* current_adr = initRAMstruct.heap_begin;
        unsigned int* old_adr = current_adr;
        do {
            old_adr = current_adr;
            current_adr += *(++current_adr)/4;
            ++current_adr;
        } while (current_adr != bloc - 2);
        current_adr = old_adr+2;
        return current_adr;
    }

    unsigned int* get_next_bloc(unsigned int* bloc){
        return (bloc-1)+(*(bloc-1)/4)+3;
    }

    void init_bloc(unsigned int* ALLOC_ADR){
        for (unsigned int i = 0; i != *(ALLOC_ADR - 1)/4; ++i){
            *(ALLOC_ADR + i) = 0;
        }
        return;
    }

#endif