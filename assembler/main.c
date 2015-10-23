#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../processor/commands.h"

#define DEFAULT_INPUT "source.in"
#define DEFAULT_OUTPUT "code.out"

#define NOT_COMMAND 0
#define SIMPLE_COMMAND 1
#define COMPLEX_COMMAND 2

#define MY_NAME "GavYur"
#define VERSION "0.1"
#define PRINT_VER(program) printf(program " v" VERSION " (%s %s) by " MY_NAME "\n", __DATE__, __TIME__)

int assemble_code(const char* inputfile, const char* outputfile);
int count_commands(FILE* stream, int* commands_cnt, int* params_cnt);
int is_command(char* str);
int fill_commands(FILE* stream, CPU_command_t* commands);
int get_cmd_number(char* str);
int write_assembled(const char* filename, const CPU_command_t* commands, int commands_cnt, int params_cnt);
int print_help();
int print_version();

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        return assemble_code(DEFAULT_INPUT, DEFAULT_OUTPUT);
    } else if (argc == 2)
    {
        if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h"))
            return print_help();
        else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v"))
            return print_version();
        else
        {
            char* inputname = argv[1];
            char* outputname = (char*) calloc(strlen(inputname) + 4, sizeof(*outputname));
            strcat(outputname, inputname);
            strcat(outputname, ".out");
            int result = assemble_code(inputname, outputname);
            free(outputname);
            return result;
        }
    } else if (argc == 3)
    {
        if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h") || !strcmp(argv[2], "--help") || !strcmp(argv[2], "-h"))
            return print_help();
        else if (!strcmp(argv[1], "--version") || !strcmp(argv[1], "-v") || !strcmp(argv[2], "--version") || !strcmp(argv[2], "-v"))
            return print_version();
        else
            return assemble_code(argv[1], argv[2]);
    } else
    {
        return print_help();
    }
}

int print_help()
{
    PRINT_VER("Assembler");
    printf("\nusage: assembler [options] [input_file] [output_file]\n\n"
           "Options:\n"
           "  -h, --help\t\tprints this message\n"
           "  -v, --version\t\tprints version of this program\n\n"
           "If no input and output file specified, program will use \"%s\" as input file and \"%s\" as output.\n"
           "If only input file specified, program will use input_file + \".out\" as output\n",
           DEFAULT_INPUT, DEFAULT_OUTPUT);

    return 0;
}

int print_version()
{
    PRINT_VER("Assembler");

    return 0;
}

int assemble_code(const char* inputfile, const char* outputfile)
{
    assert(inputfile);
    assert(outputfile);

    FILE* stream = fopen(inputfile, "rb");
    if (!stream)
    {
        printf("Error opening file ");
        perror(inputfile);
        return 1;
    }

    int commands_cnt = 0;
    int params_cnt = 0;
    if (count_commands(stream, &commands_cnt, &params_cnt) != 0)
        return 2;

    CPU_command_t* commands = (CPU_command_t*) calloc(commands_cnt, sizeof(*commands));

    if (fill_commands(stream, commands) != 0)
        return 3;

    if (write_assembled(outputfile, commands, commands_cnt, params_cnt) != 0)
    {
        printf("Error writing assembled code to ");
        perror(outputfile);
        return 4;
    }

    printf("Assembled code has successfully written to %s!\n", outputfile);

    fclose(stream);
    free(commands);

    return 0;
}

int count_commands(FILE* stream, int* commands_cnt, int* params_cnt)
{
    char wrd[64] = {};
    while (fscanf(stream, "%64s", wrd) != EOF)
    {
        int is_cmd = is_command(wrd);
        if (!is_cmd)
        {
            printf("Incorrect command %s found\n", wrd);
            rewind(stream);
            return -1;
        }
        int tmp_num = 0;
        if (is_cmd == COMPLEX_COMMAND)
        {
            if (fscanf(stream, "%d", &tmp_num) == 1)
            {
                ++(*params_cnt);
            } else
            {
                fscanf(stream, "%32s", wrd);
                if (strcmp(wrd, "rax") != 0)
                {
                    printf("Incorrect command %s found\n", wrd);
                    rewind(stream);
                    return -1;
                }
            }
        }
        ++(*commands_cnt);
    }
    rewind(stream);
    return 0;
}

int is_command(char* str)
{
    if (!strcmp(str, "push") ||
        !strcmp(str, "pop") ||
        !strcmp(str, "ja") ||
        !strcmp(str, "jmp"))
        return COMPLEX_COMMAND;
    if (!strcmp(str, "add") ||
        !strcmp(str, "sub") ||
        !strcmp(str, "mul") ||
        !strcmp(str, "div") ||
        !strcmp(str, "pow") ||
        !strcmp(str, "out") ||
        !strcmp(str, "end"))
        return SIMPLE_COMMAND;
    return NOT_COMMAND;
}

int fill_commands(FILE* stream, CPU_command_t* commands)
{
    int cmd_index = 0;
    char wrd[64] = {};
    while (fscanf(stream, "%64s", wrd) != EOF)
    {
        int cmd = get_cmd_number(wrd);
        CPU_command_t command = {};
        CPU_command_ctor(&command, cmd, 0);
        int param = 0;
        switch (cmd)
        {
        case PUSH:
            if (fscanf(stream, "%d", &param) == 1)
            {
                command.parameter = param;
            } else
            {
                fscanf(stream, "%32s", wrd); // reads 'rax', there would be only 'rax' because all incorrect words we didn't pass in count_commands()
                command.command = PUSH_RAX;
            }
            break;
        case POP:
            if (fscanf(stream, "%d", &param) == 1)
            {
                printf("Incorrect argument for pop command found: %d\n", param);
                rewind(stream);
                return -1;
            } else
            {
                fscanf(stream, "%32s", wrd); // reads 'rax'
            }
            break;
        case JA:
        case JMP:
            if (fscanf(stream, "%d", &param) == 1)
            {
                command.parameter = param;
            } else
            {
                fscanf(stream, "%32s", wrd); // reads 'rax'
                printf("Incorrect argument for %s command found: %s\n", cmd == JA ? "ja" : "jmp", wrd);
                rewind(stream);
                return -1;
            }
            break;
        default:
            break;
        }
        commands[cmd_index] = command;
        ++cmd_index;
    }
    return 0;
}

int get_cmd_number(char* str)
{
    if (!strcmp(str, "push"))
        return PUSH;
    else if (!strcmp(str, "pop"))
        return POP;
    else if (!strcmp(str, "ja"))
        return JA;
    else if (!strcmp(str, "jmp"))
        return JMP;
    else if (!strcmp(str, "add"))
        return ADD;
    else if (!strcmp(str, "sub"))
        return SUB;
    else if (!strcmp(str, "mul"))
        return MUL;
    else if (!strcmp(str, "div"))
        return DIV;
    else if (!strcmp(str, "pow"))
        return POW;
    else if (!strcmp(str, "out"))
        return OUT;
    else if (!strcmp(str, "end"))
        return END;
    return -1;
}

int write_assembled(const char* filename, const CPU_command_t* commands, int commands_cnt, int params_cnt)
{
    assert(filename);
    assert(commands);
    assert(commands_cnt > 0);

    FILE* stream = fopen(filename, "wb");
    if (!stream)
        return 1;

    fprintf(stream, "%d ", commands_cnt);
    fprintf(stream, "%d ", params_cnt);

    for (int i = 0; i < commands_cnt; ++i)
    {
        CPU_command_t cmd = commands[i];
        fprintf(stream, "%d ", cmd.command);
        if ((cmd.command == PUSH) || (cmd.command == JA) || (cmd.command == JMP))
            fprintf(stream, "%d ", cmd.parameter);
    }
    fprintf(stream, "\n");

    fclose(stream);

    return 0;
}
