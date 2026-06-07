#include "include/terminal.h"
#include "../../kernel/src/include/kernel.h"
#include "../../kernel/src/include/string.h"
#include "../../kernel/src/include/drivers/keyboard/kb_tools.h"
#include "../../kernel/src/include/types.h"
#include "../../kernel/src/include/vga/stdio.h"

char argument[2048];
struct builtinCommands builtin_commands;

void next_entry(int clear){
    init_builtin_commands();

    init_keys();
    if (clear >= 1){
        clear_screen();
    }
    print_string("--------------------------------------------------------------------------------", 0x0F, &position);
    unsigned char centerX = (80-length("Executed Built-in shell"))/2;
    for (unsigned int i = 0; i != centerX; ++i){
        print_char(' ', 0x00, &position);
    }
    printf("\033\16Executed Built-in shell\n\033\x0F");
    unsigned char ret = 0;
    while (!ret){
        ret = prompt(&position);
    }
    return;
}

void init_builtin_commands(){
    builtin_commands.commands[0]        = "clear";
    builtin_commands.builtin_adr[0]     = (unsigned int*)&clear;
    builtin_commands.needs_args[0]      = 0;
    builtin_commands.arg_count_min[0]   = 0;
    builtin_commands.arg_count_max[0]   = 0;
    builtin_commands.help[0]            = "Clears shell's screen";

    builtin_commands.commands[1]        = "help";
    builtin_commands.builtin_adr[1]     = (unsigned int*)&help;
    builtin_commands.needs_args[1]      = 0;
    builtin_commands.arg_count_min[1]   = 0;
    builtin_commands.arg_count_max[1]   = 0;
    builtin_commands.help[1]            = "Prints This help screen";


    builtin_commands.commands[2]        = 0;
    return;
}

char* command_args[256];

unsigned char prompt(volatile unsigned int *position){
    printf("\n[90HzOS@krnl]$ \033\xF0%c\033\x0F", 0);
    --*(position);
    struct output trans_key;
    unsigned char key=0;
    unsigned Oldkey = key;
    char full_command[4096];
    full_command[0] = 0;
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
                --*(position);
                printf("\033\x00  \033\x0F");
                struct command Command = parse(full_command);
                if (Command.rcode != 0){
                    com_err(Command);
                }
                else if (Command.com_adr != 0x00){
                    exec(Command.com_adr, Command.arguments);
                }
                return 0;
            }
            else {
                continue;
            }
        }
        if (trans_key.Ctrlpressed && trans_key.char1 == 'q'){
            return 1;
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
            printf("%s\033\xF0%c\033\x0F", full_command, 0);
            continue;
        }
        if (!trans_key.released && trans_key.char1 != 0 && !trans_key.extended){
            *(full_command + command_pos) = trans_key.char1;
            command_pos += 1;
            *(position) = prompt_pos;
            *(full_command + command_pos) = 0;
            printf("%s\033\xF0%c\033\x0F", full_command, 0);
            continue;
        }
    }
}

struct command parse(char* full_command){
    struct command Com;
    replace_string(Com.full_command, full_command);
    Com.rcode = OK;
    Com.com_adr = 0x00;
    *(Com.arguments) = 0;

    char command[256] = "";
    unsigned int com_idx = 0;
    *(full_command + length(full_command)) = 0;
    while (com_idx != length(full_command) && *(full_command + com_idx) != ' '){
        *(command + com_idx) = *(full_command + com_idx);
        ++com_idx;
    }

    replace_string(Com.command, command);
    *(command + com_idx) = 0;
    *argument = 0;
    if (com_idx != length(full_command)){
        ++com_idx;
    }

    unsigned int arg_idx = 0;
    unsigned int args_idx = 0;

    for (unsigned int i = 0; *(full_command + com_idx + i) != 0; ++i){
        if (*(full_command + com_idx + i) != ' '){
            *(argument + arg_idx) = *(full_command + com_idx + i);
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
    if (*(argument) != 0){
        *(argument + arg_idx) = 0;
        *(Com.arguments + args_idx) = argument;
        arg_idx = 0;
        ++args_idx;
        *(Com.arguments + args_idx) = 0;
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
        if (!*(builtin_commands.needs_args + avail_com_idx) && length_arrptr(Com.arguments) > 0){
            Com.rcode = Takes_No_Arg;
            return Com;
        }
        else if (*(builtin_commands.needs_args + avail_com_idx) && length_arrptr(Com.arguments) > *(builtin_commands.arg_count_max + avail_com_idx)){
            Com.rcode = Too_Many_Arg;
            return Com;
        }
        else if (*(builtin_commands.needs_args + avail_com_idx) && length_arrptr(Com.arguments) < *(builtin_commands.arg_count_min + avail_com_idx)){
            Com.rcode = Missing_Arg;
            return Com;
        }
    }
    Com.com_adr = (unsigned int*)*(builtin_commands.builtin_adr + avail_com_idx);
    return Com;
}

void com_err(struct command Com){
    switch (Com.rcode){
        case 1:
            printf("\nUnknown command:\033\x04 %s\033\x0F", Com.command);
            break;
        case 2:
            printf("\n%s: \033\x04Takes No Argument.\033\x0F", Com.command);
            break;
        case 3:
            printf("\n%s: \033\x04Missing Agument(s).\033\x0F", Com.command);
            break;
        case 4:
            printf("\n%s: \033\x04Too Many Args Were Given.\033\x0F", Com.command);
            break;
        default:
            return;
    }
    return;
};

void clear(){
    clear_screen();
    return;
}

void help(){
    printf("\n\033\x06============================== [BUILTIN COMMANDS] ==============================");
    for (unsigned int i = 0; *(builtin_commands.commands + i) != 0; ++i){
        printf("\n\033\x01%s\033\x0F: %s", *(builtin_commands.commands + i), *(builtin_commands.help + i));
    }
    printf("\n\n\033\x06================================================================================\033\x0F");
    return;
}