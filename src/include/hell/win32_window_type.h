#ifndef HELL_WIN32_WINDOW_TYPE_H
#define HELL_WIN32_WINDOW_TYPE_H

#include <windows.h>

typedef struct {
    HINSTANCE hinstance;
    HWND      hwnd;
} Win32Window;

#endif