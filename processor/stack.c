#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "myassert.h"

int Stack_ctor(Stack_t* This, int size)
{
    assert(This);
    assert(size > 0);

    This->size = size;
    This->count = 0;
    This->data = (float*) calloc(size, sizeof(*This->data));

    ASSERT_OK(Stack, This);

    return 0;
}

int Stack_dtor(Stack_t* This)
{
    assert(This);

    This->size = 0;
    This->count = -1;
    free(This->data);
    This->data = 0;

    return 0;
}

int Stack_ok(Stack_t* This)
{
    if (!This)
        return 0;
    if (This->size <= 0)
        return 0;
    if (This->count < 0)
        return 0;
    if (!This->data)
        return 0;
    if (This->count > This->size)
        return 0;
    return 1;
}

int Stack_dump(Stack_t* This, char* name)
{
    assert(This);

    printf("%s = Stack_t(%s)\n"
           "{\n"
           "    size = %d\n"
           "    count = %d\n"
           "    data = \n"
           "    {\n",
           name, Stack_ok(This) ? "ok" : "NOT OK!!!", This->size, This->count);
    if (This->data)
        for (int i = 0; i < This->size; ++i)
            printf("        [%d]%s%g\n", i, i < This->count ? " " : " * ", This->data[i]);
    else
        printf("        NULL pointer here :(\n");
    printf("    }\n"
           "}\n");

    return 0;
}

int Stack_push(Stack_t* This, float value)
{
    ASSERT_OK(Stack, This);

    This->data[This->count] = value;
    ++This->count;

    ASSERT_OK(Stack, This);
    return 0;
}

float Stack_pop(Stack_t* This)
{
    ASSERT_OK(Stack, This);

    float value = 0;
    if (This->count > 0)
        value = This->data[This->count - 1];
    //This->data[This->count - 1] = 0;
    --This->count;

    ASSERT_OK(Stack, This);
    return value;
}
