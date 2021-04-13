#include "mem.h"
#include <string.h>

void* hell_m_Alloc(size_t size)
{
    return malloc(size);
}

void hell_m_Free(void* ptr)
{
    free(ptr);
}

char* hell_m_CopyString(const char* in)
{
    char* out = hell_m_Alloc(strlen(in) + 1);
    strcpy(out, in);
    return out;
}
