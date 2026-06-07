#ifndef TERMINAL_H
    #define TERMINAL_H

    void next_entry(int clear);

        unsigned char prompt(volatile unsigned int *position);
        void init_builtin_commands();
        void clear();
        void help();

        struct builtinCommands {
            char*           commands[3];
            unsigned int*   builtin_adr[3];
            char            needs_args[3];
            unsigned int    arg_count_min[3];
            unsigned int    arg_count_max[3];
            char*           help[3];
        };

        enum commands_rcode{
            OK = 0,
            Unknown = 1,
            Takes_No_Arg = 2,
            Missing_Arg = 3,
            Too_Many_Arg = 4,
        };

        struct command {
            unsigned int* com_adr;
            char full_command            [4096];
            char command                 [64];
            char* arguments     [64];
            enum commands_rcode rcode;
        };

        struct command parse(char* full_command);

        void com_err(struct command Com);
        extern void exec(unsigned int* com_adr, char** arguments);
        extern volatile unsigned int position;

#endif