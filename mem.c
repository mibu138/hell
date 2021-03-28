#include "mem.h"

void* H_Malloc(size_t size)
{
    return malloc(size);
}

void H_Free(void* ptr)
{
    free(ptr);
}
