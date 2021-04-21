#include "common.h"
#include <string.h>
#include <stdlib.h>

void* hell_Malloc(size_t size)
{
    return malloc(size);
}

void hell_Free(void* ptr)
{
    free(ptr);
}

char* hell_CopyString(const char* in)
{
    char* out = hell_Malloc(strlen(in) + 1);
    strcpy(out, in);
    return out;
}
