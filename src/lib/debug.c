#include "debug.h"
#include "common.h"
#include "cmd.h"
#include "io.h"

#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define MAX_FILTERED_TAGS 32
#define MAX_DEBUG_MSG_LEN 4096

static const char* filteredDebugTags[MAX_FILTERED_TAGS];
static int filterTagCount;

static char debugMsg[MAX_DEBUG_MSG_LEN];


void hell_AddFilterTag(const char* tag)
{
    for (int i = 0; i < filterTagCount; i++)
    {
        if (strcmp(filteredDebugTags[i], tag) == 0) // tag already in list
            return;
    }
    filteredDebugTags[filterTagCount++] = tag;
    assert(filterTagCount < MAX_FILTERED_TAGS);
}

void hell_AddFilterTags(const unsigned int count, const char* tags[count])
{
    for (int j = 0; j < count; j++)
    {
        for (int i = 0; i < filterTagCount; i++)
        {
            if (strcmp(filteredDebugTags[i], tags[j]) == 0) // tag already in list
                goto next;
        }
        filteredDebugTags[filterTagCount++] = tags[j];
        assert(filterTagCount < MAX_FILTERED_TAGS);
next:
        continue;
    }
}

// idea here is we always call this function in group of 3.
// if we match the filter with one of our tags then we skip the next 2 calls.
#define VALVE_OFF_POSITION 2
void hell_DebugPrintImpl(const char* fmt, ...)
{
    static int valve = 0;
    if (valve)
    {
        valve--;
        return;
    }
    for (int i = 0; i < filterTagCount; i++)
    {
        if (strcmp(fmt, filteredDebugTags[i]) == 0)
        {
            valve = VALVE_OFF_POSITION;
            return;
        }
    }
    va_list args;
    va_start(args, fmt);
    vsnprintf(debugMsg, MAX_DEBUG_MSG_LEN, fmt, args);
    va_end(args);
    //const Hell_C_Var* silent = hell_GetVar("debug_silent", "0", HELL_C_VAR_ARCHIVE_BIT);
    if (true)
        fputs(debugMsg, stderr);
    hell_WriteToLog(debugMsg);
}

