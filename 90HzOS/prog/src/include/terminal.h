#ifndef TERMINAL_H
    #define TERMINAL_H

    #ifdef INCLUDE_FROM_KRNL
        void next_entry(int clear);
    #endif

    #ifndef INCLUDE_FROM_KRNL
        void prompt(volatile unsigned int *position);
        void init_builtin_commands();
        void clear();

        struct builtinCommands {
            char*           commands[2];
            unsigned int*   builtin_adr[2];
            char            needs_args[1];
            unsigned int    arg_count_min[1];
            unsigned int    arg_count_max[1];
            char*           help[2];
        };

        struct builtinCommands builtin_commands;

        enum commands_rcode{
            OK = 0,
            Unknown = 1,
            Takes_No_Arg = 2,
            Missing_Arg = 3,
            Too_Many_Arg = 4,
        };

        struct command {
            char full_command   [4096];
            char command        [64];
            unsigned char* arguments     [64];
            enum commands_rcode rcode;
        };

        struct command parse(char* full_command);

        void com_err(struct command Com);
    #endif

#endif