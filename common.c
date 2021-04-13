#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "common.h"
#include "input.h"
#include "platform.h"
#include "cmd.h"
#include "window.h"
#include <unistd.h>

#ifdef UNIX
#include <dlfcn.h>
#elif defined(WINDOWS)
#include <winbase.h>
#endif

#define MAX_PRINT_MSG 256

char errorMsgBuffer[MAX_PRINT_MSG];

#define HEADER "HELL: "

void hell_Print(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    va_start(argptr, fmt);
    vsnprintf(msg, sizeof(msg), fmt, argptr);
    va_end(argptr);
    fputs(msg, stdout);
}

void hell_DPrint(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    va_start(argptr, fmt);
    vsnprintf(msg, sizeof(msg), fmt, argptr);
    va_end(argptr);
    fputs(msg, stderr);
}

void hell_Announce(const char* fmt, ...)
{
    va_list argptr;
    char    msg[MAX_PRINT_MSG];
    va_start(argptr, fmt);
    vsnprintf(msg, sizeof(msg), fmt, argptr);
    va_end(argptr);
    fputs(HEADER, stdout);
    fputs(msg, stdout);
}

void hell_Abort(void)
{
    abort();
}

void hell_Error( Hell_ErrorCode errorCode, const char *fmt, ... )
{
	va_list		argptr;
	va_start (argptr,fmt);
	vsprintf (errorMsgBuffer,fmt,argptr);
	va_end (argptr);
    fputs("***ERROR: ***\n", stderr);
    fputs(errorMsgBuffer, stderr);
    fputc('\n', stderr);
    fputs("Errno: ", stderr);
    fputc(errno, stderr);
    fputc('\n', stderr);
    hell_Abort();
}

uint64_t hell_Align(const uint64_t quantity, const uint32_t alignment)
{
    assert(alignment != 0);
    if (quantity % alignment != 0) // not aligned
        return (quantity / alignment + 1) * alignment;
    else
        return quantity;
}

void hell_BitPrint(const void* const thing, const uint32_t bitcount)
{
    int mask;
    for (int i = bitcount - 1; i >= 0; i--) {
        mask = 1 << i;   
        if (mask & *(int*)thing)
            putchar('1');
        else
            putchar('0');
    }
    putchar('\n');
}

void hell_BytePrint(const void* const thing, const uint32_t byteCount)
{
    int mask;
    const uint8_t* base = (uint8_t*)thing;
    for (int i = byteCount - 1; i >= 0; i--) 
    {
        for (int j = 8 - 1; j >= 0; j--) 
        {
            mask = 1 << j;
            if (mask & *(base + i))
                putchar('1');
            else
                putchar('0');
        }
    }
    putchar('\n');
}

void hell_Print_Vec3(const float v[3])
{
    hell_Print("[ %f, %f, %f]", v[0], v[1], v[2]);
}

void hell_Print_Mat4(const float m[4][4])
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            printf("%f ", m[i][j]);
        }
        printf("\n");
    }
}

void*    hell_LoadLibrary(const char* name)
{
    #ifdef UNIX
    return dlopen(name, RTLD_LAZY);
    #elif defined(WINDOWS)
    return LoadLibrary(name);
    #endif
}

void*    hell_LoadSymbol(void* module, const char* symname)
{
    #ifdef UNIX
    return dlsym(module, symname);
    #elif defined(WINDOWS)
    return GetProcAddress(module, symname);
    #endif
}

bool hell_FileExists(const char* path)
{
    if (access(path, F_OK) == 0)
        return true;
    else 
        return false;
}

void hell_Sleep(double s)
{
    uint64_t us = (s * 1000000);
    usleep(us);
}
