#define HELL_SIMPLE_TYPE_NAMES
#define HELL_SIMPLE_FUNCTION_NAMES
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
#include "types.h"
#include "vars.h"
#include <stdio.h>

typedef Hell_Var Var;

static void dummyUserFrame(Hell_Frame fi, Hell_Tick dt)
{
    // no op we call in case user frame is not provided
    // may be an optimization? gets rid of an if statement in the loop
    // because we always call something
}

int hell_save_recorded_input(Hell_Mouth* hm, const char* filepath)
{
    int err = 0, num;
    FILE* fp = fopen(filepath, "wb");
    if (!fp) {err = -1; goto end;}

    Hell_Event* events = hm->recorded_input.elems;
    int count = hm->recorded_input.count;

    for (int i = 0; i < count; ++i) {
        num = fwrite(events + i, sizeof(*events), 1, fp);
        if (num != 1) {err = -1; goto end;}
    }

end:
    if (fp)
        fclose(fp);
    return err;
}

static void save_input_cmd(Hell_Grimoire* grim, void* data)
{
    hell_save_recorded_input(data, "recorded_input.hell");
}

// not used anymore... should see about removing it
void
hell_CreateHellmouth(Hell_Grimoire* grimoire, Hell_EventQueue* queue, Hell_Console* console,
                     uint32_t windowCount, Hell_Window* windows[],
                     Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown,
                     Hell_Mouth* hellmouth)
{
    memset(hellmouth, 0, sizeof(Hell_Mouth));
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
    hellmouth->recorded_input = hell_create_array(sizeof(Hell_Event), 4, hell_Realloc);
    hell_AddCommand(grimoire, "quit", hell_Quit, hellmouth);
    hell_AddCommand(grimoire, "save_input", save_input_cmd, hellmouth);
    const Hell_Var* dedicated = hell_GetVar(grimoire, "dedicated", 0, 0);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Hellmouth created.\n");
}

int
hell_OpenMouth(Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown, uint32_t option_flags, Hell_Mouth* hm)
{
    hm->eventqueue = hell_AllocEventQueue();
    hm->grimoire   = hell_AllocGrimoire();
    hm->userShutDown = userShutDown;
    hm->userFrame  = userFrame ? userFrame : dummyUserFrame;
    hm->recorded_input = hell_create_array(sizeof(Hell_Event), 4, hell_Realloc);
    hm->console = NULL;

    hell_CreateEventQueue(hm->eventqueue);
    hell_CreateGrimoire(hm->eventqueue, hm->grimoire);

    if ((option_flags & HELL_OPTION_ENABLE_TTY_CONSOLE_BIT))
    {
        hm->console = hell_AllocConsole();
        hell_CreateConsole(hm->console);
    }
    else
    {
        Print("HELL_NO_TTY defined! No terminal\n");
        hm->console = NULL;
    }

    hell_AddCommand(hm->grimoire, "quit", hell_Quit, hm);
    hell_AddCommand(hm->grimoire, "save_input", save_input_cmd, hm);
    const Hell_Var* dedicated = hell_GetVar(hm->grimoire, "dedicated", 0, 0);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Hellmouth created.\n");
    return 0;
}

Hell_Window*
hell_HellmouthAddWindow(Hell_Mouth* hm, u16 w, u16 h, const char* name)
{
    const u32 i = hm->windowCount++;
    // note realloc behaves as malloc if hm->windows == 0
    hm->windows = hell_Realloc(hm->windows, hm->windowCount * sizeof(Hell_Window*));
    hm->windows[i] = hell_AllocWindow();
    hell_CreateWindow(hm->eventqueue, w, h, name, hm->windows[i]);
    return hm->windows[i];
}

void hell_Frame(Hell_Mouth* h, Tick delta)
{
    hell_PushFrameEvent(h->eventqueue, h->frameCount);
    hell_CoagulateInput(h->eventqueue, h->console, h->windowCount, h->windows);
    hell_RecordInput(h->eventqueue, &h->recorded_input);
    hell_SolveInput(h->eventqueue, h->frameEventStack, &h->frameEventCount);
    hell_Incantate(h->grimoire);
}

const Hell_Event*
hell_GetEvents(Hell_Mouth* h, int* event_count)
{
    *event_count = h->frameEventCount;
    return h->frameEventStack;
}

static Tick
fpsToFrameDur(double fps)
{
    return (1.0 / fps) * 1e6;
}

void hell_Loop(Hell_Mouth* h)
{
    Tick start, delta, target;
    h->frameEventStack = hell_Malloc(sizeof(Hell_Event) * MAX_QUEUE_EVENTS);

    // vars should never be removed once an application starts, so it is safe to
    // hold onto a pointer to one.
    const Hell_Var* var_fps = hell_GetVar(h->grimoire, "fps", 60.0, 0);

    hell_StartClock();
    hell_Announce("Entering Hell Loop.\n");
    delta = fpsToFrameDur(var_fps->value);

    for (;;) {
        start = hell_Time();
        h->frameEventCount = 0;
        hell_Frame(h, delta);
        h->userFrame(h->frameCount, delta);
        h->frameCount++;
        delta = hell_Time() - start;
        target = fpsToFrameDur(var_fps->value);
        hell_MicroSleep(MAX(target - delta, 0));
        delta = hell_Time() - start;
    }
}

void hell_DestroyHellmouth(Hell_Mouth* h)
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
    Hell_Mouth* hellmouth = (Hell_Mouth*)hellmouthvoid;
    if (hellmouth->userShutDown)
        hellmouth->userShutDown();
    hell_DestroyHellmouth(hellmouth);
    exit(0);
}

void hell_Exit(int code)
{
    exit(code);
}

void hell_CloseHellmouth(Hell_Mouth* hellmouth)
{
    if (hellmouth->userShutDown)
        hellmouth->userShutDown();
    hell_DestroyHellmouth(hellmouth);
}

void hell_CloseAndExit(Hell_Mouth* hm)
{
    hell_CloseHellmouth(hm);
    hell_Exit(0);
}

uint64_t hell_SizeOfHellmouth(void)
{
    return sizeof(Hell_Mouth);
}

Hell_Mouth* hell_AllocHellmouth(void)
{
    return hell_Malloc(sizeof(Hell_Mouth));
}
