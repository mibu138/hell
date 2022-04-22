#ifndef HELL_DS_H
#define HELL_DS_H

#define STBDS_NO_SHORT_NAMES
#include "types.h"
#include "stb_ds.h"
#include "common.h"

typedef void* (*HellAllocFn)(u64 size);
// note that a proper realloc function needs to also handle case when size if 0
// since this will be called to free array
typedef void* (*HellReallocFn)(void*, u64 size);
typedef void  (*HellFreeFn)(void*);

typedef struct Hell_Array {
    u32           count;
    void*         elems;
    u32           capacity;
    u32           elemSize;
    HellReallocFn reallocFn;
} Hell_Array;

typedef struct Hell_StackArray {
    u32           count;
    void*         elems;
    u32           capacity;
    u32           elemSize;
    HellReallocFn reallocFn;
} Hell_StackArray;

typedef Hell_Array HellArray;

// if userAlloc or userRealloc is null will default to Hell_Malloc or Hell_Realloc
void  hell_CreateArray(u32 capacity, u32 elemSize, HellAllocFn userAlloc, HellReallocFn userRealloc, Hell_Array* array);
HellArray hell_create_array(size_t elem_size, size_t capacity, HellReallocFn);
int   hell_array_putc(HellArray*, char c);
void  hell_ArrayPush(Hell_Array*, const void* elem);
void  hell_ArrayPop(Hell_Array*, void* target);

// set count back to 0 and zeroes out elems
void  hell_ArrayClear(Hell_Array* arr);
// if userAlloc is null will default to Hell_Free
void  hell_DestroyArray(Hell_Array*, HellFreeFn userFree);

// returns ptr to element inside the array or null on error
void* hell_array_push(HellArray* arr, const void* elem);

void hell_array_free(HellArray* arr);

// new array based on stbds_array

#define hell_ArrLen      stbds_arrlen
#define hell_ArrLenu     stbds_arrlenu
#define hell_ArrPut      stbds_arrput
#define hell_ArrPush     stbds_arrput
#define hell_ArrPop      stbds_arrpop
#define hell_ArrFree     stbds_arrfree
#define hell_ArrAddnPtr  stbds_arraddnptr
#define hell_ArrAddnIndex stbds_arraddnindex
#define hell_ArrSetLen   stbds_arrsetlen
#define hell_ArrLast     stbds_arrlast
#define hell_ArrIns      stbds_arrins
#define hell_ArrInsn     stbds_arrinsn
#define hell_ArrDel      stbds_arrdel
#define hell_ArrDeln     stbds_arrdeln
#define hell_ArrDelSwap  stbds_arrdelswap
#define hell_ArrCap      stbds_arrcap
#define hell_ArrSetCap   stbds_arrsetcap

#endif /* end of include guard: HELL_DS_H */
