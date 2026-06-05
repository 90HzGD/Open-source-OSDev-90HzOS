#ifndef TERMINAL_H
    #define TERMINAL_H

    #ifdef INCLUDE_FROM_KRNL
        void next_entry(int clear);
    #endif

    #ifndef INCLUDE_FROM_KRNL
        void prompt(volatile unsigned int *position);
        unsigned int parse(volatile unsigned int* position, char* full_command);
        void init_builtin_commands();

        struct builtinCommands {
            char* commands[2];
            char  needs_args[1];
            unsigned int   arg_count_min[1];
            unsigned int   arg_count_max[1];
        };

        struct builtinCommands builtin_commands;
    #endif

#endif