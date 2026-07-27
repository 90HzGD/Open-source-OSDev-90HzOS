#ifndef MEM_ALLOCH

    #define MEM_ALLOCH

    char* malloc(unsigned int size);
    void free(unsigned int* ALLOC_ADR);
    void write_string(unsigned char* string, unsigned int* STR_TARGET);
    unsigned int get_remain_heap_RAM(unsigned char mode);
    unsigned int* get_previous_bloc(unsigned int* bloc);
    unsigned int* get_next_bloc(unsigned int* bloc);
    void init_bloc(unsigned int* ALLOC_ADR);                // Overrides bloc content w/ only zeros
    char* alloc_str(char* str);
    void free_str(char** str_adr);

#endif