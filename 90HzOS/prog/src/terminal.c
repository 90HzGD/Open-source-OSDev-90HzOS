#include "include/terminal.h"
#include "../../kernel/src/include/kernel.h"
#include "../../kernel/src/include/string.h"
#include "../../kernel/src/include/drivers/keyboard/kb_tools.h"

void next_entry(int clear){
    extern volatile unsigned int position;
    init_keys();
    if (clear >= 1){
        clear_screen(&position);
    }
    print_string("--------------------------------------------------------------------------------Executed Terminal", 0x0F, &position);

    prompt(&position);
    return;
}

/*struct output {
        unsigned char char1;
        unsigned char char2;
        unsigned char char3;
        unsigned char char4;
        unsigned char char5;
        unsigned char char6;
        unsigned char Altpressed;
        unsigned char Ctrlpressed;
        unsigned char ifchar;
        unsigned char released;
    };*/

char* command_args[256];

void prompt(volatile unsigned int *position){
    print_string("\n[90HzOS@krnl]$ ", 0x0F, position);
    print_char(0, 0xF0, position);
    --*(position);
    struct output trans_key;
    unsigned char key=0;
    unsigned Oldkey = key;
    char full_command[4096];
    unsigned int command_pos = 0;
    unsigned int prompt_pos = *(position);
    while (1){
        Oldkey = key;
        key = get_key();
        if (key == Oldkey || key == 0){
            continue;
        }
        trans_key = transkey(key);
        if (trans_key.char1 == '\r' || trans_key.char1 == '\t'){
            if (trans_key.char1 == '\r' && !trans_key.released){
                while (!trans_key.released){
                    key = get_key();
                    trans_key = transkey(key);
                }
                if (command_pos == 0){
                    print_char(0, 0x00, position);
                }
                else {
                    --*(position);
                    print_char(0, 0x0F, position);
                }
                unsigned int rcode = parse(position, full_command);
                if (rcode == 0){
                    prompt(position);
                }
                return;
            }
            else {
                continue;
            }
        }
        if (trans_key.Ctrlpressed && trans_key.char1 == 'q'){
            return;
        }
        if (trans_key.char1 == '\x08' && !trans_key.released){
            unsigned int len = length(full_command);
            if (len == 0){
                continue;
            }
            command_pos -= 1;
            full_command[len-1] = '\0';
            --*position;
            print_char(0, 0x00, position);
            *(position) = prompt_pos;
            print_string(full_command, 0x0F, position);
            print_char(0, 0xF0, position);
            continue;
        }
        if (!trans_key.released && trans_key.char1 != 0 && !trans_key.extended){
            *(full_command + command_pos) = trans_key.char1;
            command_pos += 1;
            *(position) = prompt_pos;
            *(full_command + command_pos) = 0;
            print_string(full_command, 0x0F, position);
            print_char(0, 0xF0, position);
            continue;
        }
    }
}

unsigned int parse(volatile unsigned int* position, const char* full_command){
    char command[256] = "";
    unsigned int com_idx = 0;
    while (com_idx != length(full_command) && *(full_command + com_idx) != ' '){
        *(command + com_idx) = *(full_command + com_idx);
        ++com_idx;
    }
    *(command + com_idx) = 0;

    if (compare_string(command, "\0")){
        return 0;
    }
    else if (compare_string(command, "clear")){
        clear_screen(position);
        return 0;
    }
    else {
        print_string("\nUnknown command: ", 0x0F, position);
        print_string(command, 0x04, position);
        return 0;
    }
}