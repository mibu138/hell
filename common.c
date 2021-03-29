#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "common.h"

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
