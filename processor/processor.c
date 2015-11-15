#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include "commands.h"
#include "processor.h"
#include "myassert.h"
#include "stack.h"

int CPU_ctor(CPU_t* This)
{
    assert(This);

    This->rax = 0;
    This->rbx = 0;
    This->rcx = 0;
    This->rdx = 0;
    This->cstack = (Stack_t*) calloc(1, sizeof(*This->cstack));
    Stack_ctor(This->cstack, STACK_SIZE);

    ASSERT_OK(CPU, This);

    return 0;
}

int CPU_dtor(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    This->rax = 0;
    This->rbx = 0;
    This->rcx = 0;
    This->rdx = 0;
    Stack_dtor(This->cstack);
    free(This->cstack);
    This->cstack = 0;

    return 0;
}

int CPU_ok(CPU_t* This)
{
    if (!This)
        return 0;
    if (!Stack_ok(This->cstack))
        return 0;
    return 1;
}

int CPU_dump(CPU_t* This, char* name)
{
    assert(This);

    printf("%s = CPU_t(%s)\n"
           "{\n"
           "    rax = %g\n"
           "    rbx = %g\n"
           "    rcx = %g\n"
           "    rdx = %g\n",
           name, CPU_ok(This) ? "ok" : "NOT OK!!!", This->rax, This->rbx, This->rcx, This->rdx);
    if (This->cstack)
        Stack_dump(This->cstack, "cstack");
    else
        printf("    cstack = 0!!!\n");
    printf("}\n");

    return 0;
}

int CPU_push(CPU_t* This, float value)
{
    ASSERT_OK(CPU, This);

    Stack_push(This->cstack, value);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_push_var(CPU_t* This, float var)
{
    if (var == RAX)
        CPU_push(This, This->rax);
    else if (var == RBX)
        CPU_push(This, This->rbx);
    else if (var == RCX)
        CPU_push(This, This->rcx);
    else if (var == RDX)
        CPU_push(This, This->rdx);
    else
        return -1;

    return 0;
}

int CPU_pop(CPU_t* This, float var)
{
    ASSERT_OK(CPU, This);

    if (var == 0)
        This->rax = Stack_pop(This->cstack);
    else if (var == 1)
        This->rbx = Stack_pop(This->cstack);
    else if (var == 2)
        This->rcx = Stack_pop(This->cstack);
    else if (var == 3)
        This->rdx = Stack_pop(This->cstack);
    else
        return -1;

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_ja(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    if (a > b)
        CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_jae(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    if (a >= b)
        CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_jb(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    if (a < b)
        CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_jbe(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    if (a <= b)
        CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_je(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    if (a == b)
        CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_jne(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    if (a != b)
        CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_jmp(CPU_t* This, float param, int* current_command)
{
    ASSERT_OK(CPU, This);

    *current_command = param - 1;

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_call(CPU_t* This, float param, int* current_command, Stack_t* call_stack)
{
    ASSERT_OK(CPU, This);

    Stack_push(call_stack, *current_command + 1);
    CPU_jmp(This, param, current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_ret(CPU_t* This, int* current_command, Stack_t* call_stack)
{
    ASSERT_OK(CPU, This);

    CPU_jmp(This, Stack_pop(call_stack), current_command);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_add(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    CPU_push(This, a + b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_sub(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    CPU_push(This, a - b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_mul(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    CPU_push(This, a * b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_div(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    CPU_push(This, a / b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_pow(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    float b = Stack_pop(This->cstack);
    CPU_push(This, pow(a, b));

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_dup(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float a = Stack_pop(This->cstack);
    CPU_push(This, a);
    CPU_push(This, a);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_in(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    float value = 0;
    printf("Input parameter> ");
    scanf("%f", &value);
    CPU_push(This, value);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_out(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    printf("%g\n", Stack_pop(This->cstack));

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_run_program(CPU_t* This, CPU_command_t* commands)
{
    ASSERT_OK(CPU, This);

    Stack_t call_stack = {};
    Stack_ctor(&call_stack, CALL_STACK_SIZE);

    int command_index = 0;
    CPU_command_t command = commands[command_index];
    while (command.command != END)
    {
        switch (command.command)
        {
        case PUSH:
            CPU_push(This, command.parameter);
            break;
        case PUSH_VAR:
            CPU_push_var(This, command.parameter);
            break;
        case POP:
            CPU_pop(This, command.parameter);
            break;
        case JA:
            CPU_ja(This, command.parameter, &command_index);
            break;
        case JAE:
            CPU_jae(This, command.parameter, &command_index);
            break;
        case JB:
            CPU_jb(This, command.parameter, &command_index);
            break;
        case JBE:
            CPU_jbe(This, command.parameter, &command_index);
            break;
        case JE:
            CPU_je(This, command.parameter, &command_index);
            break;
        case JNE:
            CPU_jne(This, command.parameter, &command_index);
            break;
        case JMP:
            CPU_jmp(This, command.parameter, &command_index);
            break;
        case CALL:
            CPU_call(This, command.parameter, &command_index, &call_stack);
            break;
        case RET:
            CPU_ret(This, &command_index, &call_stack);
            break;
        case ADD:
            CPU_add(This);
            break;
        case SUB:
            CPU_sub(This);
            break;
        case MUL:
            CPU_mul(This);
            break;
        case DIV:
            CPU_div(This);
            break;
        case POW:
            CPU_pow(This);
            break;
        case DUP:
            CPU_dup(This);
            break;
        case IN:
            CPU_in(This);
            break;
        case OUT:
            CPU_out(This);
            break;
        default:
            break;
        }
        ++command_index;
        command = commands[command_index];
    }

    return 0;
}
