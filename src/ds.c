#include "ds.h"
#include "common.h"
#include <string.h>
#include <assert.h>

static void growArray(Hell_Array* stack)
{
    assert(stack->capacity < UINT32_MAX / 2);
    u32 newCap = stack->capacity * 2;
    stack->elems = stack->reallocFn(stack->elems, newCap);
    if (!stack->elems) 
        hell_Error(HELL_ERR_FATAL, "Stack growth failed.\n");
    stack->capacity = newCap;
}

static void* arrayPtr(Hell_Array* stack)
{
    return (u8*)stack->elems + stack->count * stack->elemSize;
}

static void* arrayLastElemPtr(Hell_Array* stack)
{
    return (u8*)stack->elems + (stack->count - 1) * stack->elemSize;
}

// if userAlloc is null will default to Hell_Malloc
void  hell_CreateArray(u32 capacity, u32 elemSize, HellAllocFn userAlloc, HellReallocFn userRealloc, Hell_Array* array)
{
    assert(array);
    assert(capacity > 0);
    assert(elemSize > 0);
    memset(array, 0, sizeof(*array));
    array->capacity = capacity;
    array->elemSize = elemSize;
    if (userRealloc)
        array->reallocFn = userRealloc;
    else 
        array->reallocFn = hell_Realloc;
    if (userAlloc) 
        array->elems = userAlloc(capacity * elemSize);
    else 
        array->elems = hell_Malloc(capacity * elemSize);
    if (!array->elems) 
        hell_Error(HELL_ERR_FATAL, "Stack allocation failed.\n");
}

void hell_ArrayPush(Hell_Array* stack, const void* elem)
{
    void* stkptr = arrayPtr(stack);
    memcpy(stkptr, elem, stack->elemSize);
    stack->count++;
    if (stack->count >= stack->capacity)
        growArray(stack);
}

void  hell_ArrayPop(Hell_Array* stack, void* target)
{
    assert(stack->count > 0);
    void* lastel= arrayLastElemPtr(stack);
    memcpy(target, lastel, stack->elemSize);
    stack->count--;
}

void  hell_ArrayClear(Hell_Array* arr)
{
    memset(arr->elems, 0, arr->capacity);
    arr->count = 0;
}

// if userAlloc is null will default to Hell_Free
void  hell_DestroyArray(Hell_Array* stack, HellFreeFn userFree)
{
    if (userFree)
        userFree(stack->elems);
    else 
        hell_Free(stack->elems);
    memset(stack, 0, sizeof(*stack));
}

