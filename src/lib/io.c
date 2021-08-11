#include <stdio.h>
#include <string.h>
#include "common.h"
#include "cmd.h"
#include "platform.h"
#include <sys/stat.h>
#include <assert.h>
#if defined(WIN32)
#include "fileapi.h"
#elif defined(UNIX)
#include <sys/types.h>
#include <unistd.h>
#define UNIX_LOG_DIR  "/tmp/hell"
#define UNIX_LOG_PATH UNIX_LOG_DIR"/"LOG_NAME
#define LOG_PATH UNIX_LOG_PATH
#else 
#error "Need to find an os specific place to put the log file"
#endif

static int globalFileCounter;
static FILE* hell_log;
// 
// https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-gettemppatha?redirectedfrom=MSDN
// should be large enough for windows and linux. windows docs list max length of temp dir path is 261
#define PATH_MAX 300
static char logpathbuf[PATH_MAX]; 

#define LOG_NAME "log.txt"
#define MAX_MSG_LEN 4096

static void
createDir(void)
{
#if defined(UNIX)
    createDir(UNIX_LOG_DIR);
#elif defined(WIN32)
    DWORD pathlen = GetTempPathA(PATH_MAX, logpathbuf);
    assert(pathlen);
    strcpy(logpathbuf + pathlen, "/hell"); // tack on our dir
#else 
#error "Not supported on this os yet"
#endif
    struct stat st = {0};
    if (stat(logpathbuf, &st) == -1)
    {
        int er = mkdir(logpathbuf, 0700);
        if (er)
            fprintf(stderr, HELL_ERR_FATAL, "Error creating log directory\n");
    }
    strcat(logpathbuf, "/"LOG_NAME);
}

void hell_InitLogger(void)
{
#ifdef WIN32
    return;
#endif
    // only open the log file on first call
    if (globalFileCounter) return;
    createDir();
    if (globalFileCounter == 0)
    {
        hell_log = fopen(logpathbuf, "w");
        if (!hell_log)
            hell_Error(HELL_ERR_FATAL, "Failed to open %s.\n", LOG_NAME);
    }
    globalFileCounter++;
}

void hell_ShutdownLogger(void)
{
#ifdef WIN32
    return;
#endif
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
    // too much trouble to get this right
#ifdef WIN32
    return;
#endif
    // probably want a lock here so multiple threads could write to log
    hell_InitLogger(); // so we can call this without explicitly starting up hell
    int l = strnlen(msg, MAX_MSG_LEN);
    if (l == MAX_MSG_LEN)
        hell_Error(HELL_ERR_FATAL, "Message too long\n");
    int r = fwrite(msg, l, 1, hell_log);
    if (r == 0)
        hell_Error(HELL_ERR_MILD, "Write to log failed.\n");
}
