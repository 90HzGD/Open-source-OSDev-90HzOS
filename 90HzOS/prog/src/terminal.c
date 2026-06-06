#include "include/terminal.h"
#include "../../kernel/src/include/kernel.h"
#include "../../kernel/src/include/string.h"
#include "../../kernel/src/include/drivers/keyboard/kb_tools.h"
#include "../../kernel/src/include/types.h"

void next_entry(int clear){
    init_builtin_commands();

    extern volatile unsigned int position;
    init_keys();
    if (clear >= 1){
        clear_screen(&position);
    }
    print_string("--------------------------------------------------------------------------------Executed Terminal", 0x0F, &position);

    prompt(&position);
    return;
}

void init_builtin_commands(){
    builtin_commands.commands[0]        = "help";
    *(builtin_commands.builtin_adr)     = (unsigned int*)&clear;
    *(builtin_commands.needs_args)      = 0;
    *(builtin_commands.arg_count_min)   = 0;
    *(builtin_commands.arg_count_max)   = 0;
    builtin_commands.help[0]            = "Prints This help screen";

    builtin_commands.commands[1]        = "clear";
    *(builtin_commands.needs_args)      = 0;
    *(builtin_commands.arg_count_min)   = 0;
    *(builtin_commands.arg_count_max)   = 0;
    builtin_commands.help[1]            = "Clears shell's screen";

    builtin_commands.commands[2]        = 0;
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
                struct command Command = parse(full_command);
                if (Command.rcode != 0){
                    com_err(Command);
                }
                prompt(position);
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

struct command parse(char* full_command){
    struct command Com;
    replace_string(Com.full_command, full_command);
    Com.rcode = OK;

    char command[256] = "";
    unsigned int com_idx = 0;
    *(full_command + length(full_command)) = 0;
    while (com_idx != length(full_command) && *(full_command + com_idx) != ' '){
        *(command + com_idx) = *(full_command + com_idx);
        ++com_idx;
    }

    replace_string(Com.command, command);
    *(command + com_idx) = 0;
    unsigned char argument[2048];
    *argument = 0;

    unsigned int arg_idx = 0;
    unsigned int args_idx = 0;

    for (unsigned int i = 0; *(full_command + com_idx + i) != 0; ++i){
        if (*(full_command + com_idx + i) != ' '){
            *(argument + i) = *(full_command + com_idx + i);
            ++arg_idx;
            continue;
        }
        else {
            *(argument + arg_idx) = 0;
            *(Com.arguments + args_idx) = argument;
            arg_idx = 0;
            ++args_idx;
            *(Com.arguments + args_idx) = 0;
            continue;
        }
    }

    unsigned int avail_com_idx = 0;
    if (compare_string(command, "\0")){
        return Com;
    }
    else {
        enum boolean avail_command = 0;
        for (unsigned int i = 0; *(builtin_commands.commands + i) != 0; ++i){
            avail_command = compare_string(command, *(builtin_commands.commands + i));
            if (avail_command){
                avail_com_idx = i;
                break;
            }
        }
        if (!avail_command){
            Com.rcode = Unknown;
            return Com;
        }
        if (!*(builtin_commands.needs_args + avail_com_idx) && length((char*)Com.arguments) > 0){
            Com.rcode = Takes_No_Arg;
            return Com;
        }
    }
    return Com;
}

void com_err(struct command Com){
    extern volatile unsigned int position;
    switch (Com.rcode){
        case 1:
            print_string("\nUnknown Command: ", 0x0F, &position);
            print_string(Com.command, 0x04, &position);
            break;
        case 2:
            print_char('\n', 0x0F, &position);
            print_string(Com.command, 0x04, &position);
            print_string(": Takes No Argument!", 0x0F, &position);
            break;
        case 3:
            print_char('\n', 0x0F, &position);
            print_string(Com.command, 0x04, &position);
            print_string(": Missing Argument(s).", 0x0F, &position);
            break;
        case 4:
            print_char('\n', 0x0F, &position);
            print_string(Com.command, 0x04, &position);
            print_string(": Too Many Args were given.", 0x0F, &position);
            break;
        default:
            return;
    }
    return;
};

void clear(){
    return;
}

void help(){
    return;
}