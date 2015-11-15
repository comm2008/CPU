#ifndef ASM_COMMANDS_H_INCLUDED
#define ASM_COMMANDS_H_INCLUDED

// registers
#define RAX 0
#define RBX 1
#define RCX 2
#define RDX 3

enum COMMAND {
    END = 0,
    PUSH = 1,
    PUSH_VAR = 2,
    POP = 3,
    JA = 4,
    JAE = 5,
    JB = 6,
    JBE = 7,
    JE = 8,
    JNE = 9,
    JMP = 10,
    CALL = 11,
    RET = 12,
    ADD = 13,
    SUB = 14,
    MUL = 15,
    DIV = 16,
    POW = 17,
    DUP = 18,
    IN = 19,
    OUT = 20,
    NOP = 21
};

typedef struct
{
    int command;
    float parameter;
} CPU_command_t;

int CPU_command_ctor(CPU_command_t* This, int command, float param);
int CPU_command_dtor(CPU_command_t* This);
int CPU_command_ok(CPU_command_t* This);
int CPU_command_dump(CPU_command_t* This, char* name);

#endif // ASM_COMMANDS_H_INCLUDED
