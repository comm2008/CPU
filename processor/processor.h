#ifndef ASM_INTERPRETER_H_INCLUDED
#define ASM_INTERPRETER_H_INCLUDED

#include "commands.h"
#include "stack.h"

#define STACK_SIZE 100

typedef struct
{
    long long rax;
    Stack_t* cstack;
} CPU_t;

int CPU_ctor(CPU_t* This);
int CPU_dtor(CPU_t* This);
int CPU_ok(CPU_t* This);
int CPU_dump(CPU_t* This, char* name);
int CPU_push(CPU_t* This, int value);
int CPU_pop(CPU_t* This);
int CPU_add(CPU_t* This);
int CPU_sub(CPU_t* This);
int CPU_mul(CPU_t* This);
int CPU_div(CPU_t* This);
int CPU_pow(CPU_t* This);
int CPU_out(CPU_t* This);
int CPU_run_program(CPU_t* This, CPU_command_t* commands);

#endif // ASM_INTERPRETER_H_INCLUDED
