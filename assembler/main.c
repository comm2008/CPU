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

#define STR_(x) #x
#define STR(x) STR_(x)
#define MAX_LABELNAME 64

#define MY_NAME "GavYur"
#define VERSION "0.1"
#define PRINT_VER(program) printf(program " v" VERSION " (%s %s) by " MY_NAME "\n", __DATE__, __TIME__)

typedef struct
{
    char* name;
    int length;
    int index;
} Label;

int assemble_code(const char* inputfile, const char* outputfile);
int count_commands(FILE* stream, int* commands_cnt, int* params_cnt, int* labels_cnt);
int is_command(char* str);
int fill_commands(FILE* stream, CPU_command_t* commands, Label* labels, int labels_cnt, int warn_label);
int get_cmd_number(char* str);
int str_lower(char* str);
int write_assembled(const char* filename, const CPU_command_t* commands, int commands_cnt, int params_cnt);
int print_help();
int print_version();
int label_ctor(Label* This, const char* str, int index);
int label_destruct(Label* This);
int find_index_by_labelname(Label* labels, int labels_cnt, const char* name);

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

int label_ctor(Label* This, const char* str, int index)
{
    assert(This);
    This->name = strdup(str);
    if (!This->name)
        return 1;
    This->length = strlen(str);
    This->index = index;
    return 0;
}

int label_destruct(Label* This)
{
    assert(This);
    free(This->name);
    This->name = 0;
    This->length = -1;
    This->index = -1;
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
    int labels_cnt = 0;
    if (count_commands(stream, &commands_cnt, &params_cnt, &labels_cnt) != 0)
        return 2;

    Label* labels = (Label*) calloc(labels_cnt, sizeof(*labels));
    CPU_command_t* commands = (CPU_command_t*) calloc(commands_cnt, sizeof(*commands));

    if (fill_commands(stream, commands, labels, labels_cnt, 0) != 0)
        return 3;
    if (fill_commands(stream, commands, labels, labels_cnt, 1) != 0)
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
    for (int i = 0; i < labels_cnt; ++i)
        label_destruct(&labels[i]);
    free(labels);

    return 0;
}

int str_lower(char* str)
{
    for (int i = 0; str[i]; ++i)
        str[i] = tolower(str[i]);
    return 0;
}

int find_index_by_labelname(Label* labels, int labels_cnt, const char* name)
{
    for (int i = 0; i < labels_cnt; ++i)
        if (labels[i].name && !strcmp(labels[i].name, name))
            return labels[i].index;
    return -1;
}

int count_commands(FILE* stream, int* commands_cnt, int* params_cnt, int* labels_cnt)
{
    char wrd[MAX_LABELNAME] = {};
    while (fscanf(stream, "%" STR(MAX_LABELNAME) "s", wrd) != EOF)
    {
        wrd[MAX_LABELNAME - 1] = '\0';
        str_lower(wrd);
        int is_cmd = is_command(wrd);
        if (is_cmd)
        {
            float tmp_fl = 0;
            if (is_cmd == COMPLEX_COMMAND)
            {
                if (fscanf(stream, "%f", &tmp_fl) != 1)
                {
                    char param[MAX_LABELNAME] = {};
                    fscanf(stream, "%" STR(MAX_LABELNAME) "s", param);
                    param[MAX_LABELNAME - 1] = '\0';
                    str_lower(param);
                    if (!strcmp(wrd, "push") || !strcmp(wrd, "pop"))
                    {
                        if (strcmp(param, "rax") &&
                            strcmp(param, "rbx") &&
                            strcmp(param, "rcx") &&
                            strcmp(param, "rdx"))
                        {
                            printf("Incorrect parameter %s found\n", param);
                            rewind(stream);
                            return -1;
                        }
                    } else if (!strcmp(wrd, "ja") ||
                               !strcmp(wrd, "jae") ||
                               !strcmp(wrd, "jb") ||
                               !strcmp(wrd, "jbe") ||
                               !strcmp(wrd, "je") ||
                               !strcmp(wrd, "jne") ||
                               !strcmp(wrd, "jmp"))
                    {
                        if (param[strlen(param) - 1] != ':')
                        {
                            printf("Incorrect parameter %s found\n", param);
                            rewind(stream);
                            return -1;
                        }
                    }
                }
                ++(*params_cnt);
            }
            ++(*commands_cnt);
        } else if (wrd[strlen(wrd) - 1] == ':')
        {
            ++(*labels_cnt);
        } else
        {
            printf("Incorrect command %s found\n", wrd);
            rewind(stream);
            return -1;
        }
    }
    rewind(stream);
    return 0;
}

int is_command(char* str)
{
    if (!strcmp(str, "push") ||
        !strcmp(str, "pop") ||
        !strcmp(str, "ja") ||
        !strcmp(str, "jae") ||
        !strcmp(str, "jb") ||
        !strcmp(str, "jbe") ||
        !strcmp(str, "je") ||
        !strcmp(str, "jne") ||
        !strcmp(str, "jmp") ||
        !strcmp(str, "call"))
        return COMPLEX_COMMAND;
    if (!strcmp(str, "add") ||
        !strcmp(str, "sub") ||
        !strcmp(str, "mul") ||
        !strcmp(str, "div") ||
        !strcmp(str, "pow") ||
        !strcmp(str, "dup") ||
        !strcmp(str, "out") ||
        !strcmp(str, "nop") ||
        !strcmp(str, "in") ||
        !strcmp(str, "ret") ||
        !strcmp(str, "end"))
        return SIMPLE_COMMAND;
    return NOT_COMMAND;
}

int fill_commands(FILE* stream, CPU_command_t* commands, Label* labels, int labels_cnt, int warn_label)
{
    int cmd_index = 0;
    int lbl_index = 0;
    int lenwrd = 0;
    char wrd[MAX_LABELNAME] = {};
    while (fscanf(stream, "%" STR(MAX_LABELNAME) "s", wrd) != EOF)
    {
        wrd[MAX_LABELNAME - 1] = '\0';
        lenwrd = strlen(wrd);
        if (wrd[lenwrd - 1] == ':')
        {
            wrd[lenwrd - 1] = '\0';
            Label label = {};
            label_ctor(&label, wrd, cmd_index);
            labels[lbl_index] = label;
            ++lbl_index;
        } else
        {
            str_lower(wrd);
            int cmd = get_cmd_number(wrd);
            CPU_command_t command = {};
            CPU_command_ctor(&command, cmd, 0);
            float param = 0;
            switch (cmd)
            {
            case PUSH:
                if (fscanf(stream, "%f", &param) == 1)
                {
                    command.parameter = param;
                } else
                {
                    fscanf(stream, "%" STR(MAX_LABELNAME) "s", wrd);
                    command.command = PUSH_VAR;
                    wrd[63] = '\0';
                    str_lower(wrd);
                    if (!strcmp(wrd, "rax"))
                        command.parameter = RAX;
                    else if (!strcmp(wrd, "rbx"))
                        command.parameter = RBX;
                    else if (!strcmp(wrd, "rcx"))
                        command.parameter = RCX;
                    else if (!strcmp(wrd, "rdx"))
                        command.parameter = RDX;
                }
                break;
            case POP:
                if (fscanf(stream, "%f", &param) == 1)
                {
                    printf("Incorrect argument for pop command found: %g\n", param);
                    rewind(stream);
                    return -1;
                } else
                {
                    fscanf(stream, "%" STR(MAX_LABELNAME) "s", wrd);
                    wrd[MAX_LABELNAME - 1] = '\0';
                    str_lower(wrd);
                    if (!strcmp(wrd, "rax"))
                        command.parameter = RAX;
                    else if (!strcmp(wrd, "rbx"))
                        command.parameter = RBX;
                    else if (!strcmp(wrd, "rcx"))
                        command.parameter = RCX;
                    else if (!strcmp(wrd, "rdx"))
                        command.parameter = RDX;
                }
                break;
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
                } else
                {
                    fscanf(stream, "%" STR(MAX_LABELNAME) "s", wrd);
                    wrd[MAX_LABELNAME - 1] = '\0';
                    lenwrd = strlen(wrd);
                    if ((wrd[lenwrd - 1] == ':') || (cmd == CALL))
                    {
                        if (wrd[lenwrd - 1] == ':')
                            wrd[lenwrd - 1] = '\0';
                        int index = find_index_by_labelname(labels, labels_cnt, wrd);
                        command.parameter = index;
                        if ((index == -1) && warn_label)
                        {
                            printf("Unknown label found: %s\n", wrd);
                            rewind(stream);
                            return -1;
                        }
                    } else
                    {
                        printf("Incorrect argument found: %s\n", wrd);
                        rewind(stream);
                        return -1;
                    }
                }
                break;
            default:
                break;
            }
            commands[cmd_index] = command;
            ++cmd_index;
        }
    }
    rewind(stream);
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
    else if (!strcmp(str, "jae"))
        return JAE;
    else if (!strcmp(str, "jb"))
        return JB;
    else if (!strcmp(str, "jbe"))
        return JBE;
    else if (!strcmp(str, "je"))
        return JE;
    else if (!strcmp(str, "jne"))
        return JNE;
    else if (!strcmp(str, "jmp"))
        return JMP;
    else if (!strcmp(str, "call"))
        return CALL;
    else if (!strcmp(str, "ret"))
        return RET;
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
    else if (!strcmp(str, "dup"))
        return DUP;
    else if (!strcmp(str, "in"))
        return IN;
    else if (!strcmp(str, "out"))
        return OUT;
    else if (!strcmp(str, "end"))
        return END;
    else if (!strcmp(str, "nop"))
        return NOP;
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
        if ((cmd.command == PUSH) ||
            (cmd.command == PUSH_VAR) ||
            (cmd.command == POP) ||
            (cmd.command == JA) ||
            (cmd.command == JAE) ||
            (cmd.command == JB) ||
            (cmd.command == JBE) ||
            (cmd.command == JE) ||
            (cmd.command == JNE) ||
            (cmd.command == JMP) ||
            (cmd.command == CALL))
            fprintf(stream, "%g ", cmd.parameter);
    }
    fprintf(stream, "\n");

    fclose(stream);

    return 0;
}
