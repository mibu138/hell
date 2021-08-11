#ifndef HELL_WINDOW_H
#define HELL_WINDOW_H

#include "types.h"
#ifdef WIN32
#include <windows.h>
// should be called in WinMain before trying to open a window.
void  hell_SetHinstance(HINSTANCE hinstance);
#endif

typedef struct Hell_Window Hell_Window;
typedef struct Hell_EventQueue Hell_EventQueue;

void hell_CreateWindow(Hell_EventQueue* queue, const uint16_t width, const uint16_t height, const char* name,
                  Hell_Window* window);
void               hell_DestroyWindow(Hell_Window*);
// does nothing if window is not active
void               hell_DrainWindowEvents(Hell_EventQueue*, Hell_Window*);
const void*        hell_GetXcbConnection(const Hell_Window* window);

const void*        hell_GetXcbWindowPtr(const Hell_Window* window);
Hell_WindowID      hell_GetWindowID(const Hell_Window* window);
void* hell_GetHinstancePtr(const Hell_Window* window);
void* hell_GetHwndPtr(const Hell_Window* window);

uint32_t hell_GetWindowWidth(const Hell_Window* window);
uint32_t hell_GetWindowHeight(const Hell_Window* window);

#endif /* end of include guard: DISPLAY_H */
