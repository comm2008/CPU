#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../processor/commands.h"

#define DEFAULT_INPUT "code.in"
#define DEFAULT_OUTPUT "source.out"

#define MY_NAME "GavYur"
#define VERSION "0.1"
#define PRINT_VER(program) printf(program " v" VERSION " (%s %s) by " MY_NAME "\n", __DATE__, __TIME__)

int disassemble_code(const char* inputfile, const char* outputfile);
int write_disassembled(FILE* input, FILE* output);
int print_help();
int print_version();

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        return disassemble_code(DEFAULT_INPUT, DEFAULT_OUTPUT);
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
            int result = disassemble_code(inputname, outputname);
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
            return disassemble_code(argv[1], argv[2]);
    } else
    {
        return print_help();
    }
}

int print_help()
{
    PRINT_VER("Disassembler");
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
    PRINT_VER("Disassembler");

    return 0;
}

int disassemble_code(const char* inputfile, const char* outputfile)
{
    assert(inputfile);
    assert(outputfile);

    FILE* inp = fopen(inputfile, "rb");
    if (!inp)
    {
        printf("Error opening input file ");
        perror(inputfile);
        return 1;
    }

    FILE* out = fopen(outputfile, "wb");
    if (!out)
    {
        printf("Error opening output file ");
        perror(outputfile);
        return 2;
    }

    int disasm_result = write_disassembled(inp, out);
    fclose(inp);
    fclose(out);

    if (disasm_result != 0)
    {
        if (disasm_result == -2)
            printf("Program file corrupt\n");
        return 3;
    }

    printf("Disassembled code has successfully written to %s!\n", outputfile);

    return 0;
}

#define WRITE_CMD_WITH_IARG(cmd) \
if (fscanf(input, "%d", &param) == 1) \
{ \
    fprintf(output, cmd " %d\n", param); \
    ++_params_count; \
} else \
{ \
    printf("Incorrect argument for " cmd " command\n"); \
    return -1; \
}

int write_disassembled(FILE* input, FILE* output)
{
    assert(input);
    assert(output);

    int commands_count = 0;
    int _commands_count = 0;
    int params_count = 0;
    int _params_count = 0;
    fscanf(input, "%d %d", &commands_count, &params_count);

    for (int i = 0; i < commands_count; ++i)
    {
        int cmd = 0;
        fscanf(input, "%d", &cmd);
        int param = 0;
        switch (cmd)
        {
        case PUSH:
            WRITE_CMD_WITH_IARG("push");
            break;
        case PUSH_VAR:
            if (fscanf(input, "%d", &param) == 1)
            {
                fprintf(output, "push ");
                if (param == RAX)
                    fprintf(output, "rax\n");
                else if (param == RBX)
                    fprintf(output, "rbx\n");
                else if (param == RCX)
                    fprintf(output, "rcx\n");
                else if (param == RDX)
                    fprintf(output, "rdx\n");
                else
                {
                    printf("Incorrect argument for push command\n");
                    return -1;
                }
                ++_params_count;
            } else
            {
                printf("Incorrect argument for push command\n");
                return -1;
            }
            break;
        case POP:
            if (fscanf(input, "%d", &param) == 1)
            {
                fprintf(output, "pop ");
                if (param == RAX)
                    fprintf(output, "rax\n");
                else if (param == RBX)
                    fprintf(output, "rbx\n");
                else if (param == RCX)
                    fprintf(output, "rcx\n");
                else if (param == RDX)
                    fprintf(output, "rdx\n");
                else
                {
                    printf("Incorrect argument for pop command\n");
                    return -1;
                }
                ++_params_count;
            } else
            {
                printf("Incorrect argument for pop command\n");
                return -1;
            }
            break;
        case JA:
            WRITE_CMD_WITH_IARG("ja");
            break;
        case JAE:
            WRITE_CMD_WITH_IARG("jae");
            break;
        case JB:
            WRITE_CMD_WITH_IARG("jb");
            break;
        case JBE:
            WRITE_CMD_WITH_IARG("jbe");
            break;
        case JE:
            WRITE_CMD_WITH_IARG("je");
            break;
        case JNE:
            WRITE_CMD_WITH_IARG("jne");
            break;
        case JMP:
            WRITE_CMD_WITH_IARG("jmp");
            break;
        case CALL:
            WRITE_CMD_WITH_IARG("call");
            break;
        case RET:
            fprintf(output, "ret\n");
            break;
        case ADD:
            fprintf(output, "add\n");
            break;
        case SUB:
            fprintf(output, "sub\n");
            break;
        case MUL:
            fprintf(output, "mul\n");
            break;
        case DIV:
            fprintf(output, "div\n");
            break;
        case POW:
            fprintf(output, "pow\n");
            break;
        case DUP:
            fprintf(output, "dup\n");
            break;
        case IN:
            fprintf(output, "in\n");
            break;
        case OUT:
            fprintf(output, "out\n");
            break;
        case NOP:
            fprintf(output, "nop\n");
            break;
        case END:
            fprintf(output, "end\n");
            break;
        default:
            break;
        }
        ++_commands_count;
    }
    if ((commands_count == _commands_count) && (params_count == _params_count))
        return 0;
    else
        return -2;
}
