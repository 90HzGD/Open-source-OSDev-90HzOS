#ifndef KB_TOOLS_H
    #define KB_TOOLS_H
    #define STRING_H
    #include "../../string.h"
    #include "kb_data.h"
    
    struct output extended_char(unsigned char scan_code);
    unsigned char get_key(void);
    unsigned char shift_key(unsigned char target);
    void init_keys(void);
    struct output transkey(char key);
    
#endif