#include "common.h"
#include <string.h>
#include <stdlib.h>

void* hell_Malloc(size_t size)
{
    void* ptr = malloc(size);
    if (!ptr)
        hell_Error(HELL_ERR_FATAL, "Allocation failed\n");
    return ptr;
}

void hell_Free(void* ptr)
{
    free(ptr);
}

void* hell_Realloc(void* ptr, size_t size)
{
    void* newptr = realloc(ptr, size);
    if (!newptr)
        hell_Error(HELL_ERR_FATAL, "Allocation failed\n");
    ptr = newptr;
    return ptr;
}

char* hell_CopyString(const char* in)
{
    char* out = hell_Malloc(strlen(in) + 1);
    strcpy(out, in); //includes the terminating null
    return out;
}
