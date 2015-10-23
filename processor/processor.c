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
    This->cstack = (Stack_t*) calloc(1, sizeof(*This->cstack));
    Stack_ctor(This->cstack, STACK_SIZE);

    ASSERT_OK(CPU, This);

    return 0;
}

int CPU_dtor(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    This->rax = 0;
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
           "    rax = %lld\n",
           name, CPU_ok(This) ? "ok" : "NOT OK!!!", This->rax);
    if (This->cstack)
        Stack_dump(This->cstack, "cstack");
    else
        printf("    cstack = 0!!!\n");
    printf("}\n");

    return 0;
}

int CPU_push(CPU_t* This, int value)
{
    ASSERT_OK(CPU, This);

    Stack_push(This->cstack, value);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_pop(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    This->rax = Stack_pop(This->cstack);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_add(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    int a = Stack_pop(This->cstack);
    int b = Stack_pop(This->cstack);
    CPU_push(This, a + b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_sub(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    int a = Stack_pop(This->cstack);
    int b = Stack_pop(This->cstack);
    CPU_push(This, a - b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_mul(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    int a = Stack_pop(This->cstack);
    int b = Stack_pop(This->cstack);
    CPU_push(This, a * b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_div(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    int a = Stack_pop(This->cstack);
    int b = Stack_pop(This->cstack);
    CPU_push(This, a / b);

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_pow(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    int a = Stack_pop(This->cstack);
    int b = Stack_pop(This->cstack);
    CPU_push(This, trunc(pow(a, b)));

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_out(CPU_t* This)
{
    ASSERT_OK(CPU, This);

    printf("%d\n", Stack_pop(This->cstack));

    ASSERT_OK(CPU, This);
    return 0;
}

int CPU_run_program(CPU_t* This, CPU_command_t* commands)
{
    ASSERT_OK(CPU, This);

    int command_index = 0;
    CPU_command_t command = commands[command_index];
    while (command.command != END)
    {
        switch (command.command)
        {
        case PUSH:
            CPU_push(This, command.parameter);
            break;
        case PUSH_RAX:
            CPU_push(This, This->rax);
            break;
        case POP:
            CPU_pop(This);
            break;
        case JA: ;
            int a = Stack_pop(This->cstack);
            int b = Stack_pop(This->cstack);
            if (a > b)
            {
                command_index = command.parameter - 1;
            }
            break;
        case JMP:
            command_index = command.parameter - 1;
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
