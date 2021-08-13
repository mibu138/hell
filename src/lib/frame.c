#include "common.h"
#include "cmd.h"
#include "io.h"
#include "window.h"
#include "input.h"
#include "client.h"
#include "server.h"
#include "private.h"
#include "stdlib.h"
#include <assert.h>
#include <string.h>
#define HELL_SIMPLE_TYPE_NAMES
#include "types.h"
#include "vars.h"

typedef Hell_C_Var Var;

static void dummyUserFrame(void)
{
    // no op we call in case user frame is not provided
    // may be an optimization? gets rid of an if statement in the loop
    // because we always call something
}

typedef struct Hell_Hellmouth {
    Hell_Grimoire*   grimoire;
    Hell_EventQueue* eventqueue;
    Hell_Console*    console;
    Hell_Window**    windows;
    uint32_t         windowCount;
    Hell_FrameFn     userFrame;
    Hell_ShutDownFn  userShutDown;
} Hell_Hellmouth;

void
hell_CreateHellmouth(Hell_Grimoire* grimoire, Hell_EventQueue* queue, Hell_Console* console,
                     uint32_t windowCount, Hell_Window* windows[],
                     Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown,
                     Hell_Hellmouth* hellmouth)
{
    memset(hellmouth, 0, sizeof(Hell_Hellmouth));
    assert(queue);
    assert(grimoire);
    hell_InitLogger();
    hell_Announce("Creating Hellmouth...\n");
    hellmouth->grimoire = grimoire;
    hellmouth->eventqueue = queue;
    hellmouth->console = console;
    hellmouth->windowCount = windowCount;
    hellmouth->windows = windows;
    hellmouth->userFrame = userFrame ? userFrame : dummyUserFrame;
    hellmouth->userShutDown = userShutDown;
    hell_AddCommand(grimoire, "quit", hell_Quit, hellmouth);
    const Hell_C_Var* dedicated = hell_GetVar(grimoire, "dedicated", "0", 0);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Hellmouth created.\n");
}

void hell_Frame(Hell_Hellmouth* h, Tick delta)
{
    hell_CoagulateInput(h->eventqueue, h->console, h->windowCount, h->windows);
    hell_SolveInput(h->eventqueue);
    hell_Incantate(h->grimoire);
}

void hell_Loop(Hell_Hellmouth* h)
{
    const Var* vFps = hell_GetVar(h->grimoire, HELL_VAR_NAME_MAX_FPS, "60", HELL_C_VAR_ARCHIVE_BIT);
    const double targetFrameLength = (1.0 / vFps->value);
    hell_StartClock();
    Tick startTick = hell_Time(); 
    Tick endTick = startTick;
    hell_Announce("Entering Hell Loop.\n");
    while (1)
    {
        // this is just a place holder
        hell_Sleep(targetFrameLength);
        endTick = hell_Time();
        hell_Frame(h, endTick - startTick);
        h->userFrame();
        startTick = endTick;
    }
}

void hell_DestroyHellmouth(Hell_Hellmouth* h)
{
    for (int i = 0; i < h->windowCount; i++)
    {
        hell_DestroyWindow(h->windows[i]);
    }
    hell_DestroyGrimoire(h->grimoire);
    hell_DestroyEventQueue(h->eventqueue);
    hell_DestroyConsole(h->console);
    hell_Announce("Shut Down.\n");
    hell_ShutdownLogger();
}

// if we're going to call exit it doesn't make sense to have anyshut down code run.
void hell_Quit(const Hell_Grimoire* grim, void* hellmouthvoid)
{
    Hell_Hellmouth* hellmouth = (Hell_Hellmouth*)hellmouthvoid;
    if (hellmouth->userShutDown)
        hellmouth->userShutDown();
    hell_DestroyHellmouth(hellmouth);
    exit(0);
}

uint64_t hell_SizeOfHellmouth(void)
{
    return sizeof(Hell_Hellmouth);
}

Hell_Hellmouth* hell_AllocHellmouth(void)
{
    return hell_Malloc(sizeof(Hell_Hellmouth));
}
