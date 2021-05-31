#include <stdio.h>
#include <string.h>
#include "common.h"
#include "cmd.h"

static int globalFileCounter;
static FILE* hell_log;

#define LOG_NAME "log.txt"
#define MAX_MSG_LEN 4096

void hell_InitLogger(void)
{
    // only open the log file on first call
    if (globalFileCounter == 0)
    {
        hell_log = fopen(LOG_NAME, "w");
        if (!hell_log)
            hell_Error(HELL_ERR_FATAL, "Failed to open %s.\n", LOG_NAME);
    }
    globalFileCounter++;
}

void hell_ShutdownLogger(void)
{
    globalFileCounter--;
    if (globalFileCounter == 0)
    {
        int r = fclose(hell_log);
        if (r)
            hell_Error(HELL_ERR_MILD, "Error on closing %s.\n", LOG_NAME);
    }
}

void hell_WriteToLog(const char* msg)
{
    // probably want a lock here so multiple threads could write to log
    int l = strnlen(msg, MAX_MSG_LEN);
    if (l == MAX_MSG_LEN)
        hell_Error(HELL_ERR_FATAL, "Message too long\n");
    int r = fwrite(msg, l, 1, hell_log);
    if (r == 0)
        hell_Error(HELL_ERR_MILD, "Write to log failed.\n");
}
