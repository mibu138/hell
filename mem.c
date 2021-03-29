#include "mem.h"

void* hell_m_Alloc(size_t size)
{
    return malloc(size);
}

void hell_m_Free(void* ptr)
{
    free(ptr);
}
