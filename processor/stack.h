#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

typedef struct
{
    int size;
    int count;
    float* data;
} Stack_t;

int Stack_ctor(Stack_t* This, int size);
int Stack_dtor(Stack_t* This);
int Stack_ok(Stack_t* This);
int Stack_dump(Stack_t* This, char* name);
int Stack_push(Stack_t* This, float value);
float Stack_pop(Stack_t* This);

#endif // STACK_H_INCLUDED
