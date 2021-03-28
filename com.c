#include <stdarg.h>
#include <stdio.h>
#include "com.h"

#define MAX_PRINT_MSG 256

void H_Printf(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    va_start(argptr, fmt);
    vsnprintf(msg, sizeof(msg), fmt, argptr);
    va_end(argptr);
    fputs(msg, stdout);
}
