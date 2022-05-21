#include "common.h"
#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(c,p,s) hell_Realloc(p,s)
#define STBDS_FREE(c,p) hell_Free(p)
#include "ds.h"
#include <string.h>
#include <assert.h>

static void growArray(Hell_Array* stack)
{
    assert(stack->capacity < UINT32_MAX / 2);
    u32 newCap = stack->capacity * 2;
    stack->elems = stack->reallocFn(stack->elems, newCap * stack->elemSize);
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

HellArray hell_create_array(size_t elem_size, size_t capacity, HellReallocFn reallocfn)
{
    HellArray arr = {0};
    hell_CreateArray(capacity, elem_size, NULL, reallocfn, &arr);
    return arr;
}

void hell_ArrayPush(Hell_Array* restrict stack, const void* restrict elem)
{
    void* stkptr = arrayPtr(stack);
    memcpy(stkptr, elem, stack->elemSize);
    stack->count++;
    if (stack->count >= stack->capacity)
        growArray(stack);
}

void* hell_array_push(HellArray* restrict arr, const void* restrict elem)
{
    void* stkptr = arrayPtr(arr);
    memcpy(stkptr, elem, arr->elemSize);
    arr->count++;
    if (arr->count >= arr->capacity)
    {
        growArray(arr);
        stkptr = arrayPtr(arr);
    }
    return stkptr;
}

void hell_array_remove_swap(HellArray* arr, size_t i)
{
    void* elem = (char*)arr->elems + i * arr->elemSize;
    void* last_elem = (char*)arr->elems + (arr->count - 1) * arr->elemSize;
    memcpy(elem, last_elem, arr->elemSize);
    --arr->count;
}

int   hell_array_putc(HellArray* a, char c)
{
    assert(a->elemSize == 1);
    hell_array_push(a, &c);
    return 0;
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

void hell_array_free(HellArray* arr)
{
    // check so that we don't accidentally free twice
    if (arr->elems)
        arr->reallocFn(arr->elems, 0);
    memset(arr, 0, sizeof(*arr));
}
