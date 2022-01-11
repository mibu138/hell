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
} Hell_Array;

// if userAlloc or userRealloc is null will default to Hell_Malloc or Hell_Realloc
void  hell_CreateArray(u32 capacity, u32 elemSize, HellAllocFn userAlloc, HellReallocFn userRealloc, Hell_Array* stack);
void  hell_ArrayPush(Hell_Array*, void* elem);
void  hell_ArrayPop(Hell_Array*, void* target);
// if userAlloc is null will default to Hell_Free
void  hell_DestroyArray(Hell_Array*, HellFreeFn userFree);


#endif /* end of include guard: HELL_DS_H */
