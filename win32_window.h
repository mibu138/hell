#ifndef HELL_MS_WINDOW_H
#define HELL_MS_WINDOW_H

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "display.h"
#include "win_local.h"
#include "win32_window_type.h"
#include <assert.h>
#include "evcodes.h"

static Win32Window win32Window;

#define MS_WINDOW_CLASS_NAME "myWindowClass"

// Step 4: the Window Procedure
inline static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_CLOSE: DestroyWindow(hwnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_LBUTTONDOWN: hell_i_PushMouseDownEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_LEFT); break;
        case WM_LBUTTONUP: hell_i_PushMouseUpEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_LEFT); break;
        case WM_MOUSEMOVE: hell_i_PushMouseMotionEvent(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_LEFT); break;
        case WM_SIZE: hell_i_PushWindowResizeEvent(LOWORD(lParam), HIWORD(lParam)); break;
        default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

inline static int initMsWindow(int width, int height, const char* name, Hell_Window* hellWindow)
{
    WNDCLASSEX wc;

    assert(winVars.instance);
    win32Window.hinstance = winVars.instance;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = win32Window.hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = MS_WINDOW_CLASS_NAME;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, _T("Window Registration Failed!"), "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    win32Window.hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        MS_WINDOW_CLASS_NAME,
        _T("Title of my window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, win32Window.hinstance, NULL);

    if(win32Window.hwnd == NULL)
    {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(win32Window.hwnd, SW_SHOWNORMAL);
    UpdateWindow(win32Window.hwnd);

    hellWindow->type = HELL_WINDOW_WIN32_TYPE;
    hellWindow->width = width;
    hellWindow->height = height;
    hellWindow->typeSpecificData = &win32Window;

    return 0;

    // Step 3: The Message Loop
}

inline static void drainMsEventQueue(void)
{
    MSG Msg;
    while(PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
    {
        printf("Got a message!\n");
        if ( !GetMessage (&Msg, NULL, 0, 0) ) 
        {
            assert(0 && "Quake 3 code exited here.. not sure what this issue is");
        } 
        // save the msg time, because wndprocs don't have access to the timestamp
        // g_wv.sysMsgTime = msg.time
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

}

inline static void cleanUpMs(void)
{

}

#endif