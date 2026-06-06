#ifndef STRING_H
    #define STRING_H

    void reverse_string(char* string);
    void replace_string(volatile char *string, volatile char* target);
    unsigned int length(volatile const char* string);
    int compare_string(volatile const char* string1, volatile const char* string2);

#endif