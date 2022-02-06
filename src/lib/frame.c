#include "common.h"
#include "cmd.h"
#include "io.h"
#include "window.h"
#include "input.h"
#include "minmax.h"
#include "client.h"
#include "server.h"
#include "private.h"
#include "stdlib.h"
#include <assert.h>
#include <string.h>
#define HELL_SIMPLE_TYPE_NAMES
#include "types.h"
#include "vars.h"

typedef Hell_Var Var;

static void dummyUserFrame(u64 fi, u64 dt)
{
    // no op we call in case user frame is not provided
    // may be an optimization? gets rid of an if statement in the loop
    // because we always call something
}

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
    hellmouth->targetFrameDuration = 16000; //us
    hell_AddCommand(grimoire, "quit", hell_Quit, hellmouth);
    const Hell_Var* dedicated = hell_GetVar(grimoire, "dedicated", "0", 0);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Hellmouth created.\n");
}

int
hell_OpenHellmouth(Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown, Hell_Hellmouth* hm)
{
    hm->eventqueue = hell_AllocEventQueue();
    hm->grimoire   = hell_AllocGrimoire();
    hm->console    = hell_AllocConsole();
    hm->userShutDown = userShutDown;
    hm->userFrame  = userFrame ? userFrame : dummyUserFrame;

    hell_CreateEventQueue(hm->eventqueue);
    hell_CreateConsole(hm->console);
    hell_CreateGrimoire(hm->eventqueue, hm->grimoire);

    hell_AddCommand(hm->grimoire, "quit", hell_Quit, hm);
    const Hell_Var* dedicated = hell_GetVar(hm->grimoire, "dedicated", "0", 0);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Hellmouth created.\n");
    return 0;
}

int
hell_OpenHellmouth_NoConsole(Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown, Hell_Hellmouth* hm)
{
    hm->eventqueue = hell_AllocEventQueue();
    hm->grimoire   = hell_AllocGrimoire();
    hm->console    = NULL;
    hm->userShutDown = userShutDown;
    hm->userFrame  = userFrame ? userFrame : dummyUserFrame;

    hell_CreateEventQueue(hm->eventqueue);
    hell_CreateGrimoire(hm->eventqueue, hm->grimoire);

    hell_AddCommand(hm->grimoire, "quit", hell_Quit, hm);
    const Hell_Var* dedicated = hell_GetVar(hm->grimoire, "dedicated", "0", 0);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Hellmouth created.\n");
    return 0;
}

Hell_Window*
hell_HellmouthAddWindow(Hell_Hellmouth* hm, u16 w, u16 h, const char* name)
{
    const u32 i = hm->windowCount++;
    // note realloc behaves as malloc if hm->windows == 0
    hm->windows = hell_Realloc(hm->windows, hm->windowCount * sizeof(Hell_Window*));
    hm->windows[i] = hell_AllocWindow();
    hell_CreateWindow(hm->eventqueue, w, h, name, hm->windows[i]);
    return hm->windows[i];
}

void hell_Frame(Hell_Hellmouth* h, Tick delta)
{
    hell_CoagulateInput(h->eventqueue, h->console, h->windowCount, h->windows);
    hell_SolveInput(h->eventqueue);
    hell_Incantate(h->grimoire);
}

void hell_Loop(Hell_Hellmouth* h)
{
    Tick start, delta;

    hell_StartClock();
    hell_Announce("Entering Hell Loop.\n");
    delta = h->targetFrameDuration;
    while (1)
    {
        // this is just a place holder
        start = hell_Time();
        hell_Frame(h, delta);
        h->userFrame(h->frameCount, delta);
        h->frameCount++;
        delta = hell_Time() - start;
        hell_MicroSleep(MAX(h->targetFrameDuration - delta, 0));
        delta = hell_Time() - start;
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

void hell_Quit(Hell_Grimoire* grim, void* hellmouthvoid)
{
    Hell_Hellmouth* hellmouth = (Hell_Hellmouth*)hellmouthvoid;
    if (hellmouth->userShutDown)
        hellmouth->userShutDown();
    hell_DestroyHellmouth(hellmouth);
    exit(0);
}

void hell_Exit(int code)
{
    exit(code);
}

void hell_CloseHellmouth(Hell_Hellmouth* hellmouth)
{
    if (hellmouth->userShutDown)
        hellmouth->userShutDown();
    hell_DestroyHellmouth(hellmouth);
}

void hell_CloseAndExit(Hell_Hellmouth* hm)
{
    hell_CloseHellmouth(hm);
    hell_Exit(0);
}

uint64_t hell_SizeOfHellmouth(void)
{
    return sizeof(Hell_Hellmouth);
}

Hell_Hellmouth* hell_AllocHellmouth(void)
{
    return hell_Malloc(sizeof(Hell_Hellmouth));
}
