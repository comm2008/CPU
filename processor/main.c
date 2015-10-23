#include <stdio.h>
#include <stdlib.h>
#include "processor.h"
#include "commands.h"

#define FILENAME_READ "../assembler/program.out"

#define MY_NAME "GavYur"
#define GREET(program, version) printf("#--- " program " v" version " (%s %s) by " MY_NAME "\n\n", __DATE__, __TIME__)

int fill_commands(FILE* stream, CPU_command_t* commands, int commands_cnt, int params_cnt);

int main()
{
    GREET("Processor", "0.1");

    FILE* stream = fopen(FILENAME_READ, "rb");
    if (!stream)
    {
        perror("Error opening file");
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
        int param = 0;
        switch (cmd)
        {
        case PUSH:
            if (fscanf(stream, "%d", &param) == 1)
            {
                command.parameter = param;
                ++_params_cnt;
            } else
            {
                printf("Incorrect argument for push command\n");
                return -1;
            }
            break;
        case JA:
        case JMP:
            if (fscanf(stream, "%d", &param) == 1)
            {
                command.parameter = param;
                ++_params_cnt;
            } else
            {
                printf("Incorrect argument for %s command\n", cmd == JA ? "ja" : "jmp");
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
