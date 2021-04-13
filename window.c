#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "window.h"
#include "common.h"
#include "evcodes.h"
#include "input.h"
#include "mem.h"
#include "platform.h"

#if defined(UNIX)
#include "unix_window.h"
#elif defined(WINDOWS)
#include "win32_window.h"
#include "win_local.h"
HellWinVars winVars;
#endif

static Hell_Window window;

const Hell_Window* hell_w_Init(const uint16_t width, const uint16_t height, const char* name)
{
    // once we support other platforms we can put a switch in here
    #if defined(UNIX)
    initXcbWindow(width, height, name, &window);
    #elif defined(WINDOWS)
    initMsWindow(width, height, name, &window);
    #endif
    return &window;
}

void hell_w_DrainWindowEvents(void)
{
    if (window.typeSpecificData) // used to it is active
    {
    #if defined(UNIX)
        drainXcbEventQueue(&window);
    #elif defined(WINDOWS)
        drainMsEventQueue();
    #endif
    }
}

void hell_w_CleanUp(void)
{
    #if defined(UNIX)
    cleanUpXcb();
    #elif defined(WINDOWS)
    cleanUpMs();
    #endif
    hell_Announce("Display shutdown.\n");
}
