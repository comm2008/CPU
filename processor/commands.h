#ifndef ASM_COMMANDS_H_INCLUDED
#define ASM_COMMANDS_H_INCLUDED

enum COMMAND {
    PUSH,
    PUSH_RAX,
    POP,
    JA,
    JMP,
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    OUT,
    END
};

typedef struct
{
    int command;
    int parameter;
} CPU_command_t;

int CPU_command_ctor(CPU_command_t* This, int command, int param);
int CPU_command_dtor(CPU_command_t* This);
int CPU_command_ok(CPU_command_t* This);
int CPU_command_dump(CPU_command_t* This, char* name);

#endif // ASM_COMMANDS_H_INCLUDED
