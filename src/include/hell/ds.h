#ifndef HELL_DS_H
#define HELL_DS_H

#include "types.h"

typedef void* (*HellAllocFn)(u64 size);
typedef void* (*HellReallocFn)(void*, u64 size);
typedef void  (*HellFreeFn)(void*);

typedef struct Hell_Stack {
    u32           count;
    u32           capacity;
    u32           elemSize;
    HellReallocFn reallocFn;
    void*         elems;
} Hell_Stack;

typedef struct Hell_Array {
    u32   count;
    u32   elemSize;
    void* elems;
} Hell_Array;

// if userAlloc or userRealloc is null will default to Hell_Malloc or Hell_Realloc
void  hell_CreateStack(u32 capacity, u32 elemSize, HellAllocFn userAlloc, HellReallocFn userRealloc, Hell_Stack* stack);
void  hell_StackPush(Hell_Stack*, void* elem);
void  hell_StackPop(Hell_Stack*, void* target);
// if userAlloc is null will default to Hell_Free
void  hell_DestroyStack(Hell_Stack*, HellFreeFn userFree);

// array stuff in progress...
void hell_CreateArray(u32 count, u32 elemSize, HellAllocFn userAllocFn, Hell_Array* array);

#endif /* end of include guard: HELL_DS_H */
