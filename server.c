#include "common.h"
#include "ds.h"

#define MAX_MSGLEN 1400             /* max length of a message */

Hell_Buffer netMessage;
Byte        netMessageData[MAX_MSGLEN];

void sv_Init(void)
{
    // bunch of cvars get initialized here... see yamagi quake 2 src/server/sv_main.c
    hell_BufferInit(&netMessage, netMessageData, sizeof(netMessageData));
    hell_Announce("Server Initialized.\n");
}
