#ifndef ASM_INTERPRETER_H_INCLUDED
#define ASM_INTERPRETER_H_INCLUDED

#include "commands.h"
#include "stack.h"

#define STACK_SIZE 100
#define CALL_STACK_SIZE 100

typedef struct
{
    float rax;
    float rbx;
    float rcx;
    float rdx;
    Stack_t* cstack;
} CPU_t;

int CPU_ctor(CPU_t* This);
int CPU_dtor(CPU_t* This);
int CPU_ok(CPU_t* This);
int CPU_dump(CPU_t* This, char* name);
int CPU_push(CPU_t* This, float value);
int CPU_push_var(CPU_t* This, float var);
int CPU_pop(CPU_t* This, float var);
int CPU_ja(CPU_t* This, float param, int* current_command);
int CPU_jae(CPU_t* This, float param, int* current_command);
int CPU_jb(CPU_t* This, float param, int* current_command);
int CPU_jbe(CPU_t* This, float param, int* current_command);
int CPU_je(CPU_t* This, float param, int* current_command);
int CPU_jne(CPU_t* This, float param, int* current_command);
int CPU_jmp(CPU_t* This, float param, int* current_command);
int CPU_call(CPU_t* This, float param, int* current_command, Stack_t* call_stack);
int CPU_ret(CPU_t* This, int* current_command, Stack_t* call_stack);
int CPU_add(CPU_t* This);
int CPU_sub(CPU_t* This);
int CPU_mul(CPU_t* This);
int CPU_div(CPU_t* This);
int CPU_pow(CPU_t* This);
int CPU_dup(CPU_t* This);
int CPU_in(CPU_t* This);
int CPU_out(CPU_t* This);
int CPU_run_program(CPU_t* This, CPU_command_t* commands);

#endif // ASM_INTERPRETER_H_INCLUDED
