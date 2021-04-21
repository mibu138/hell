#include <stdio.h>
#include <string.h>
#include "common.h"
#include "cmd.h"

static FILE* hell_log;

#define LOG_NAME "log.txt"
#define MAX_MSG_LEN 4096

static const Hell_C_Var* writeToLog;

void hell_io_Init(void)
{
    hell_log = fopen(LOG_NAME, "w");
    if (!hell_log)
        hell_Error(HELL_ERR_FATAL, "Failed to open %s.\n", LOG_NAME);
    writeToLog = hell_c_GetVar("log", "1", HELL_C_VAR_ARCHIVE_BIT);
}

void hell_io_Shutdown(void)
{
    int r = fclose(hell_log);
    if (r)
        hell_Error(HELL_ERR_MILD, "Error on closing %s.\n", LOG_NAME);
}

void hell_io_WriteToLog(const char* msg)
{
    if (!writeToLog->value)
        return;
    int l = strnlen(msg, MAX_MSG_LEN);
    if (l == MAX_MSG_LEN)
        hell_Error(HELL_ERR_FATAL, "Message too long\n");
    int r = fwrite(msg, l, 1, hell_log);
    if (r == 0)
        hell_Error(HELL_ERR_MILD, "Write to log failed.\n");
}
