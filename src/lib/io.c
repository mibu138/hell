#include <stdio.h>
#include <string.h>
#include "common.h"
#include "cmd.h"
#include "platform.h"
#ifndef UNIX
#error "Need to find a place to put log file on windows"
#endif
#ifdef UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define UNIX_LOG_DIR  "/tmp/hell"
#define UNIX_LOG_PATH UNIX_LOG_DIR"/"LOG_NAME
#define LOG_PATH UNIX_LOG_PATH
#endif

static int globalFileCounter;
static FILE* hell_log;

#define LOG_NAME "log.txt"
#define MAX_MSG_LEN 4096

static void 
createDirUnix(const char* dir)
{
    struct stat st = {0};
    if (stat(dir, &st) == -1)
    {
        int er = mkdir(dir, 0700);
        if (er)
            hell_Error(HELL_ERR_FATAL, "Error creating log directory\n");
    }
}

static void
createDir(void)
{
#ifdef UNIX
    createDirUnix(UNIX_LOG_DIR);
#endif
}

void hell_InitLogger(void)
{
    // only open the log file on first call
    if (globalFileCounter) return;
    createDir();
    if (globalFileCounter == 0)
    {
        hell_log = fopen(LOG_PATH, "w");
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
    hell_InitLogger(); // so we can call this without explicitly starting up hell
    int l = strnlen(msg, MAX_MSG_LEN);
    if (l == MAX_MSG_LEN)
        hell_Error(HELL_ERR_FATAL, "Message too long\n");
    int r = fwrite(msg, l, 1, hell_log);
    if (r == 0)
        hell_Error(HELL_ERR_MILD, "Write to log failed.\n");
}
