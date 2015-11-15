#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "processor.h"
#include "commands.h"

#define DEFAULT_INPUT "../assembler/code.out"

#define MY_NAME "GavYur"
#define GREET(program, version) printf("#--- " program " v" version " (%s %s) by " MY_NAME "\n\n", __DATE__, __TIME__)

int print_help();
int print_version();
int parse_file(const char* filename);
int fill_commands(FILE* stream, CPU_command_t* commands, int commands_cnt, int params_cnt);

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        return parse_file(DEFAULT_INPUT);
    } else if (argc == 2)
    {
        if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
            return print_help();
        else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))
            return print_version();
        else
            return parse_file(argv[1]);
    } else
        return print_help();
}
int print_help()
{
    GREET("Processor", "0.1");
    printf("\nusage: processor [options] [input_file]\n\n"
           "Options:\n"
           "  -h, --help\t\tprints this message\n"
           "  -v, --version\t\tprints version of this program\n\n"
           "If no input file specified, program will use \"%s\" as input file.\n",
           DEFAULT_INPUT);

    return 0;
}

int print_version()
{
    GREET("Processor", "0.1");

    return 0;
}

int parse_file(const char* filename)
{
    GREET("Processor", "0.1");

    FILE* stream = fopen(filename, "rb");
    if (!stream)
    {
        printf("Error opening file ");
        perror(filename);
        return 1;
    }

    int commands_cnt = 0;
    int params_cnt = 0;
    fscanf(stream, "%d %d", &commands_cnt, &params_cnt);

    CPU_command_t* commands = (CPU_command_t*) calloc(commands_cnt, sizeof(*commands));

    int fill_result = fill_commands(stream, commands, commands_cnt, params_cnt);
    fclose(stream);

    if (fill_result != 0)
    {
        if (fill_result == -2)
            printf("Program file corrupt\n");
        return 2;
    }

    CPU_t processor = {};
    CPU_ctor(&processor);

    if (CPU_run_program(&processor, commands) != 0)
    {
        printf("Runtime error\n");
        return 3;
    }

    free(commands);

    return 0;
}

int fill_commands(FILE* stream, CPU_command_t* commands, int commands_cnt, int params_cnt)
{
    assert(stream);
    assert(commands);

    int cmd_index = 0;
    int _params_cnt = 0;
    int cmd = 0;
    while (fscanf(stream, "%d", &cmd) != EOF)
    {
        CPU_command_t command = {};
        CPU_command_ctor(&command, cmd, 0);
        float param = 0;
        switch (cmd)
        {
        case PUSH:
        case PUSH_VAR:
        case POP:
        case JA:
        case JAE:
        case JB:
        case JBE:
        case JE:
        case JNE:
        case JMP:
        case CALL:
            if (fscanf(stream, "%f", &param) == 1)
            {
                command.parameter = param;
                ++_params_cnt;
            } else
            {
                printf("Incorrect argument\n");
                return -1;
            }
            break;
        default:
            break;
        }
        commands[cmd_index] = command;
        ++cmd_index;
    }
    if ((cmd_index == commands_cnt) && (_params_cnt == params_cnt))
        return 0;
    else
        return -2;
}
