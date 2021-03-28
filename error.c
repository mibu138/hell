#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

void H_Abort()
{
    abort();
}

void H_Error()
{
    fprintf(stderr, "Error has occured. Exiting. Errno: %d\n", errno);
    H_Abort();
}

void H_ErrorMsg(const char* msg)
{
    fprintf(stderr, "H_ERROR: %s\nErrno: %d\nExiting.", msg, errno);
    H_Abort();
}
