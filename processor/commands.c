#include <assert.h>
#include "myassert.h"
#include "commands.h"

int CPU_command_ctor(CPU_command_t* This, int command, int param)
{
    assert(This);

    This->command = command;
    This->parameter = param;

    ASSERT_OK(CPU_command, This);

    return 0;
}

int CPU_command_dtor(CPU_command_t* This)
{
    assert(This);

    This->command = -1;
    This->parameter = 0;

    return 0;
}

int CPU_command_ok(CPU_command_t* This)
{
    if (!This)
        return 0;
    if ((This->command < PUSH) || (This->command > END))
        return 0;

    return 1;
}

int CPU_command_dump(CPU_command_t* This, char* name)
{
    assert(This);

    printf("%s = CPU_command_t(%s)\n"
           "{\n"
           "    command = %d\n"
           "    parameter = %d\n"
           "}\n",
           name, CPU_command_ok(This) ? "ok" : "NOT OK!!!", This->command, This->parameter);

    return 0;
}
