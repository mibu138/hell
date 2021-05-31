#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "window.h"
#include "common.h"
#include "evcodes.h"
#include "input.h"
#include "platform.h"
#include "private.h"

#if defined(UNIX)
#include "unix_window.h"
#elif defined(WINDOWS)
#include "win32_window.h"
#include "win_local.h"
HellWinVars winVars;
#endif

const Hell_Window* hell_CreateWindow(const uint16_t width, const uint16_t height, const char* name, Hell_Window* window)
{
    // once we support other platforms we can put a switch in here
    memset(window, 0, sizeof(Hell_Window));
    #if defined(UNIX)
    createXcbWindow(width, height, name, window);
    #elif defined(WINDOWS)
    initMsWindow(width, height, name, window);
    #endif
    return window;
}

void hell_DrainWindowEvents(Hell_EventQueue* queue, Hell_Window* window)
{
    #if defined(UNIX)
    drainXcbEventQueue(queue, window);
    #elif defined(WINDOWS)
    drainMsEventQueue();
    #endif
}

void hell_DestroyWindow(Hell_Window* window)
{
    #if defined(UNIX)
    destroyXcbWindow(window);
    #elif defined(WINDOWS)
    destroyMsWindow();
    #endif
    memset(window, 0, sizeof(Hell_Window));
    hell_Announce("Display shutdown.\n");
}
