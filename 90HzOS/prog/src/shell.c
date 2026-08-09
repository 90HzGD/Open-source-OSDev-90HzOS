#include "include/terminal.h"
#include "../../kernel/src/include/kernel.h"
#include "../../kernel/src/include/string.h"
#include "../../kernel/src/include/drivers/keyboard/kb_tools.h"
#include "../../kernel/src/include/types.h"
#include "../../kernel/src/include/vga/stdio.h"
#include "../../kernel/src/include/mem/mem_alloc.h"
#include "../../kernel/src/include/types.h"
#include "../../kernel/src/include/drivers/PCI/PCI.h"

char argument[2048];
struct builtinCommands builtin_commands;

void next_entry(int clear){
    init_builtin_commands();

    init_keys();
    if (clear){
        clear_screen();
    }
    unsigned char ret = 0;
    while (!ret){
        ret = prompt(&position);
    }
    return;
}

void init_builtin_commands(){
    builtin_commands.commands[0]        = "clear";
    builtin_commands.builtin_adr[0]     = (unsigned int*)&clear;
    builtin_commands.needs_args[0]      = -1;
    builtin_commands.arg_count_min[0]   = 0;
    builtin_commands.arg_count_max[0]   = 0;
    builtin_commands.help[0]            = "Clears shell's screen";

    builtin_commands.commands[1]        = "help";
    builtin_commands.builtin_adr[1]     = (unsigned int*)&help;
    builtin_commands.needs_args[1]      = -1;
    builtin_commands.arg_count_min[1]   = 0;
    builtin_commands.arg_count_max[1]   = 0;
    builtin_commands.help[1]            = "Prints This help screen";

    builtin_commands.commands[2]        = "echo";
    builtin_commands.builtin_adr[2]     = (unsigned int*)&echo;
    builtin_commands.needs_args[2]      = 1;
    builtin_commands.arg_count_min[2]   = 1;
    builtin_commands.arg_count_max[2]   = 0;
    builtin_commands.help[2]            = "prints arguments to the screen";

    builtin_commands.commands[3]        = "uname";
    builtin_commands.builtin_adr[3]     = (unsigned int*)&uname;
    builtin_commands.needs_args[3]      = 0;
    builtin_commands.arg_count_min[3]   = 0;
    builtin_commands.arg_count_max[3]   = 1;
    builtin_commands.help[3]            = "prints OS info";

    builtin_commands.commands[4]        = "lspci";
    builtin_commands.builtin_adr[4]     = (unsigned int*)&lspci;
    builtin_commands.needs_args[4]      = 0;
    builtin_commands.arg_count_min[4]   = 0;
    builtin_commands.arg_count_max[4]   = 1;
    builtin_commands.help[4]            = "Displays connected PCI devices";

    builtin_commands.commands[5]        = 0;
    return;
}

char* command_args[256];
struct command Command;

unsigned char prompt(volatile unsigned int *position){
    printf("\n[90HzOS@krnl] >\033\xF0%c\033\x0F", 0);
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
                Command = parse(full_command);
                if (Command.rcode != 0){
                    com_err(Command);
                }
                else if (Command.com_adr != 0x00){
                    exec(Command.com_adr, Command.arguments);
                }
                if ((!compare_string(Command.command, "clear") || Command.rcode != 0) && (Command.com_adr != 0 || Command.rcode != 0)){
                    if (!Command.rcode){
                        printf("\n");
                    }
                    printf("Executed with rcode: \033\x03%u\033\x0F", Command.rcode);
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

    while (com_idx != length(full_command) && *(full_command + com_idx) != ' ' && com_idx < 63){
        *(command + com_idx) = *(full_command + com_idx);
        ++com_idx;
    }
    if (compare_string(command, "")){
        return Com;
    }
    unsigned int args_idx = 0;
    {
        *(command + com_idx) = 0;
        ++com_idx;
        unsigned int bypass_arg_parse = (com_idx >= length(full_command));
        replace_string(Com.command, command);
        *argument = 0;

        if (!bypass_arg_parse){
            enum boolean allow_spaces = False;
            char arg[64] = {0};
            unsigned int i = 0;
            char* str_target;
            for (;com_idx != length(full_command); ++com_idx){
                if (args_idx >= 63){
                    break;
                }
                if ((*(full_command + com_idx) != ' ' || allow_spaces) && com_idx != length(full_command) && *(full_command + com_idx) != '\"' && *(full_command + com_idx) != '\''){
                    *(arg + i) = *(full_command + com_idx);
                    ++i;
                }
                else if (*(full_command + com_idx) == '\"' || *(full_command + com_idx) == '\''){
                    allow_spaces = (allow_spaces ^ 1);
                    continue;
                }
                else {
                    *(arg + i) = 0;
                    i = 0;
                    str_target = alloc_str(arg);
                    if (!str_target){
                        *arg = 0;
                        i = 0;
                        continue;
                    }
                    *(Com.arguments + args_idx) = str_target;
                    *arg = 0;
                    ++args_idx;
                    *(Com.arguments + args_idx) = 0;
                }
            }
            *(arg+i) = 0;
            str_target = alloc_str(arg);
            *(Com.arguments + args_idx) = str_target;
            ++args_idx;
            *(Com.arguments + args_idx) = 0;
        }
    }
    int idx = -1;
    unsigned int Com_exists = in_str_arr(builtin_commands.commands, command);
    if (!Com_exists){
        Com.rcode = Unknown;
    }
    else{
        idx = search_str_arr(builtin_commands.commands, Com.command);
        Com.com_adr = *(builtin_commands.builtin_adr + idx);
    }
    if (idx >= 0){
        if (*(builtin_commands.needs_args + idx) < 0 && args_idx > 0){
            Com.rcode = Takes_No_Arg;
        }
        else if (*(builtin_commands.needs_args + idx) && *(builtin_commands.arg_count_min + idx) > args_idx){
            Com.rcode = Missing_Arg;
        }
        else if (*(builtin_commands.needs_args + idx) && *(builtin_commands.arg_count_max + idx) < args_idx && *(builtin_commands.arg_count_max + idx) != 0){
            Com.rcode = Too_Many_Arg;
        }
    }
    
    return Com;
}

char* ret[4];

char** clear(){
    *ret = (char*)OK;
    *(ret+1) = (char*)0;
    clear_screen();
    return ret;
}

char** help(){
    *ret = (char*)OK;
    *(ret+1) = (char*)0;
    printf("\n\033\x06============================== [BUILTIN COMMANDS] =============================");
    for (unsigned int i = 0; *(builtin_commands.commands + i) != 0; ++i){
        printf("\n\033\x01%s\033\x0F: %s", *(builtin_commands.commands + i), *(builtin_commands.help + i));
    }
    printf("\n\033\x06================================ [RETURN CODES] ===============================\033\x0F\n");
    printf("Rcode \033\x03");
    printf("0\033\x0F: \033\x0EOK\n\033\x0F");
    printf("Rcode \033\x03");
    printf("1\033\x0F: \033\4Command does not exist\n\033\x0F");
    printf("Rcode \033\x03");
    printf("2\033\x0F: \033\x04Gave argument(s), but command takes no argument\n\033\x0F");
    printf("Rcode \033\x03");
    printf("3\033\x0F: \033\4Did not give enough argument to command\n\033\x0F");
    printf("Rcode \033\x03");
    printf("4\033\x0F: \033\x04Gave too much arguments to command\n\033\x0F");
    printf("Rcode \033\x03");
    printf("5\033\x0F: \033\x04Invalid / Non-existing Argument\033\x0F");
    return ret;
}

char** echo(char** arguments){
    *ret = (char*)OK;
    *(ret+1) = (char*)0;
    printf("\n");
    for (unsigned int i = 0; *(arguments + i) != 0; ++i){
        printf("%s", *(arguments + i));
    }
    return ret;
}

char* avail_args[256];

char** uname(char** arguments){
    avail_args[0] = "-s";
    avail_args[1] = "--kernel-name";
    avail_args[2] = "-r";
    avail_args[3] = "--kernel-release";
    avail_args[4] = "-v";
    avail_args[5] = "--kernel-version";
    avail_args[6] = "-m";
    avail_args[7] = "--machine";
    avail_args[8] = "-o";
    avail_args[9] = "--operating-system";
    avail_args[10] = "-a";
    avail_args[11] = "--all";
    avail_args[12] = 0;
    *ret = (char*)OK;
    *(ret+1) = (char*)0;
    

    if (*arguments != 0){
        for (unsigned int i = 0; *(arguments + i) != 0; ++i){
            if (!in_str_arr(avail_args, *(arguments + i))){
                *ret = (char*)Unknown_Arg;
                *(ret + 1) = "uname";
                *(ret + 2) = *((arguments + i));
                *(ret + 3) = 0;
                return ret;
            }
        }
    }
    printf("\n");

    if (compare_string(*arguments, *(avail_args)) || compare_string(*arguments, *(avail_args + 1))){
        printf("%s", KRNL_NAME);
    }
    else if (compare_string(*(arguments), *(avail_args + 2)) || compare_string(*arguments, *(avail_args + 3))){
        printf("%s", KRNL_RELEASE);
    }
    else if (compare_string(*(arguments), *(avail_args + 4)) || compare_string(*arguments, *(avail_args + 5))){
        printf("Kernel Ver: %s", KRNL_VER);
    }
    else if (compare_string(*(arguments), *(avail_args + 6)) || compare_string(*arguments, *(avail_args + 7))){
        printf("%s 32bit protected mode (i386+)", KRNL_ARCHITECTURE);
    }
    else if (compare_string(*(arguments), *(avail_args + 8)) || compare_string(*arguments, *(avail_args + 9))){
        printf("%s", OS_NAME);
    }
    else if (compare_string(*(arguments), *(avail_args + 10)) || compare_string(*arguments, *(avail_args + 11))){
        printf("%s Ver: %s %s 32bit i386+ protected mode", KRNL_NAME, KRNL_VER, KRNL_ARCHITECTURE);
    }
    else {
        printf("%s", KRNL_NAME);
    }
    return ret;
}


char** lspci(){
    struct PCIVendors Vendors;
    fillVendors(Vendors.Vendors_str, (unsigned short*)&Vendors.VendorsID);
    *ret = (char*)OK;
    *(ret+1) = (char*)0;
    struct PCIDev_Descriptor Device;
    printf("\nEnumerating PCI devices:\n");
    for (unsigned short i = 0; i != BUS_COUNT; ++i){
        for (unsigned int j = 0; j != DEV_COUNT; ++j){
            Device = GetDevInfo((u8)i, (u8)j);
            if (Device.VENDOR_ID == 0){
                continue;
            }
            printf("Device: Bus#%u Device:#%u; ", (u32)i, j);
            int knownVendor = -1;
            for (u32 k = 0; *(Vendors.VendorsID + k) != 0; ++k){
                if (*(Vendors.VendorsID + k) == Device.VENDOR_ID){
                    knownVendor = k;
                    break;
                }
            }

            if (knownVendor != -1){
                printf("VENDOR: %s; ", *(Vendors.Vendors_str + knownVendor));
            }
            else {
                printf("VENDORID:%h; ", Device.VENDOR_ID);
            }
            printf("DEVICEID:%h\n", Device.DEVICE_ID);
        }
    }
    return ret;
}

void com_err(struct command Com){
    printf("\n\033\x07[\033\4FAIL\033\x07]\033\x0F ");
    switch (Com.rcode){
        case Unknown:
            printf("Unknown command: \033\x04%s\033\x0F\n", Com.command);
            break;
        case Takes_No_Arg:
            printf("%s: \033\x04Takes no argument.\033\x0F\n", Com.command);
            break;
        case Missing_Arg:
            printf("%s: \033\x04Too few arguments were given.\033\x0F\n", Com.command);
            break;
        case Too_Many_Arg:
            printf("%s: \033\x04Too many arguments were given.\033\x0F\n", Com.command);
            break;
        default:
            break;
    }
    return;
}

void exec_err(char** err){
    printf("\n\033\x07[\033\4FAIL\033\x07]\033\x0F ");
    switch ((enum commands_rcode)*err){
        case Unknown_Arg:
            printf("\033\x04%s\033\x0F: Unknown Argument: \033\x04%s\033\x0F\n", *(err + 1), *(err + 2));
            Command.rcode = 5;
            break;
        default:
            break;
    }
    return;
}