#include "ds.h"
#include "common.h"
#include <string.h>
#include <assert.h>

static void growStack(Hell_Stack* stack)
{
    assert(stack->capacity < UINT32_MAX / 2);
    u32 newCap = stack->capacity * 2;
    stack->elems = stack->reallocFn(stack->elems, newCap);
    if (!stack->elems) 
        hell_Error(HELL_ERR_FATAL, "Stack growth failed.\n");
    stack->capacity = newCap;
}

static void* stackPtr(Hell_Stack* stack)
{
    return (u8*)stack->elems + stack->count * stack->elemSize;
}

// if userAlloc is null will default to Hell_Malloc
void  hell_CreateStack(u32 capacity, u32 elemSize, HellAllocFn userAlloc, HellReallocFn userRealloc, Hell_Stack* stack)
{
    assert(stack);
    assert(capacity > 0);
    assert(elemSize > 0);
    memset(stack, 0, sizeof(*stack));
    stack->capacity = capacity;
    stack->elemSize = elemSize;
    if (userRealloc)
        stack->reallocFn = userRealloc;
    else 
        stack->reallocFn = hell_Realloc;
    if (userAlloc) 
        stack->elems = userAlloc(capacity * elemSize);
    else 
        stack->elems = hell_Malloc(capacity * elemSize);
    if (!stack->elems) 
        hell_Error(HELL_ERR_FATAL, "Stack allocation failed.\n");
}

void hell_StackPush(Hell_Stack* stack, void* elem)
{
    void* stkptr = stackPtr(stack);
    memcpy(stkptr, elem, stack->elemSize);
    stack->count++;
    if (stack->count >= stack->capacity)
        growStack(stack);
}

void  hell_StackPop(Hell_Stack* stack, void* target)
{
    assert(stack->count > 0);
    void* stkptr = stackPtr(stack);
    memcpy(target, stkptr, stack->elemSize);
    stack->count--;
}

// if userAlloc is null will default to Hell_Free
void  hell_DestroyStack(Hell_Stack* stack, HellFreeFn userFree)
{
    if (userFree)
        userFree(stack->elems);
    else 
        hell_Free(stack->elems);
    memset(stack, 0, sizeof(*stack));
}

void hell_CreateArray(u32 count, u32 elemSize, HellAllocFn userAlloc, Hell_Array* array)
{
    assert(array);
    assert(elemSize);
    assert(count);
    memset(array, 0, sizeof(Hell_Array));
    array->count = count;
    array->elemSize = elemSize;
    if (userAlloc)
        array->elems = userAlloc(count * elemSize);
    else 
        array->elems = hell_Malloc(count * elemSize);
}

