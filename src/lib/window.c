#include "window.h"
#include "common.h"
#include "evcodes.h"
#include "input.h"
#include "platform.h"
#include "private.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#if defined(UNIX)
#include "unix_window.h"
#elif defined(WIN32)
HellWinVars winVars;
#include "win32_window.h"
#endif

// number of windows created since program start.
// start counter at 1 so 0 is free to have special meaning
static Hell_WindowID globalWindowCounter = 1; 

static bool onWindowResize(const Hell_Event* event, void* windowPtr)
{
    Hell_Window* window = (Hell_Window*)windowPtr;
    window->width = event->data.winData.data.resizeData.width;
    window->height = event->data.winData.data.resizeData.height;
    // do not consume event 
    return false;
}

void
hell_CreateWindow(Hell_EventQueue* queue, const uint16_t width, const uint16_t height, const char* name,
                  Hell_Window* window)
{
    // once we support other platforms we can put a switch in here
    memset(window, 0, sizeof(Hell_Window));
    window->id = globalWindowCounter++;
#if defined(UNIX)
    createXcbWindow(width, height, name, window);
#elif defined(WIN32)
    createWin32Window(queue, width, height, name, window);
#endif
    assert(globalWindowCounter < HELL_WINDOW_ID_MAX);
    hell_Subscribe(queue, HELL_EVENT_MASK_WINDOW_BIT, window->id, onWindowResize, window);
}

void
hell_DrainWindowEvents(Hell_EventQueue* queue, Hell_Window* window)
{
#if defined(UNIX)
    drainXcbEventQueue(queue, window);
#elif defined(WIN32)
    drainMsEventQueue();
#endif
}

void
hell_DestroyWindow(Hell_Window* window)
{
#if defined(UNIX)
    destroyXcbWindow(window);
#elif defined(WIN32)
    destroyWin32Window(window);
#endif
    memset(window, 0, sizeof(Hell_Window));
    hell_Announce("Display shutdown.\n");
}

#ifdef UNIX
const void*
hell_GetXcbConnection(const Hell_Window* window)
{
    assert(window->type == HELL_WINDOW_XCB_TYPE);
    return ((Hell_XcbWindow*)window->typeSpecificData)->connection;
}

const void*
hell_GetXcbWindowPtr(const Hell_Window* window)
{
    assert(window->type == HELL_WINDOW_XCB_TYPE);
    return &((Hell_XcbWindow*)window->typeSpecificData)->window;
}
#endif

Hell_WindowID 
hell_GetWindowID(const Hell_Window* window)
{
    return window->id;
}

uint32_t hell_GetWindowWidth(const Hell_Window* window)
{
    return window->width;
}

uint32_t hell_GetWindowHeight(const Hell_Window* window)
{
    return window->height;
}
