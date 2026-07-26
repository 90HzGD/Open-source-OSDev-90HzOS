#include "include/string.h" 
#include "include/vga/stdio.h"
    void reverse_string(char* string){
        char* string_cpy = 0;
        unsigned int str_len = length(string);
        replace_string(string_cpy, string);
        for (unsigned int i = 0; i != str_len; ++i){
            *(string + i) = *(string_cpy + str_len - 1);
        }
    }

    void replace_string(volatile char* string, volatile char* target){
        unsigned int index=0;
        do {
            *(string+index) = *(target+index);
            ++index;
        }   while (*(target+index) != '\0');
        *(string+index) = '\0';
    }
    
    unsigned int length(volatile const char* string){
        int length = 0;
        for (unsigned int i=0; *(string+i)!='\0'; ++i){
            length += 1;
        }
        return length;
    }    

    unsigned int compare_string(volatile const char* string1, volatile const char* string2){
        unsigned int len1 = length(string1);
        unsigned int len2 = length(string2);
        if (len1 != len2){
            return 0;
        }
        else {
            for (unsigned int i=0; i<len1; ++i){
                if (string1[i] != string2[i]){
                    return 0;
                }
                continue;
            }
        }
        return 1;
    }

    unsigned char in_str_arr(char** str, char* target){
        unsigned int found = 0;
        for (unsigned int i = 0; *(str + i) != 0; ++i){
            found = compare_string(*(str + i), target);
            if (found){
                break;
            }
        }
        return found;
    }

    unsigned char search_str_arr(char** str, char* target){
        int arr_idx = -1;
        for (unsigned int found;*(str + arr_idx + 1) != 0; ++arr_idx){
            found = compare_string(*(str + arr_idx + 1), target);
            if (found){
                break;
            }
        }
        return arr_idx + 1;
    }