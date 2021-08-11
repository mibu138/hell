#ifndef HELL_MS_WINDOW_H
#define HELL_MS_WINDOW_H

#include "platform.h"
#ifdef WIN32

#include "window.h"
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "window.h"
#include "win32_window_type.h"
#include <assert.h>
#include "cmd.h"
#include "private.h"
#include "evcodes.h"
#include "common.h"
#include "win_local.h"


//static Win32Window win32Window;

#define MS_WINDOW_CLASS_NAME "myWindowClass"


typedef struct WinUserData {
    Hell_EventQueue* evqueue;
    Hell_WindowID    winId;
} WinUserData;

typedef struct {
    HINSTANCE hinstance;
    HWND      hwnd;
    WinUserData userdata;
} Win32Window;

// Step 4: the Window Procedure
inline static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static bool keyDown = false;
    static uint32_t lastKey = 0;
    WinUserData* ud = (WinUserData*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch(msg)
    {
	case WM_CREATE: SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)((CREATESTRUCT*)lParam)->lpCreateParams); break;
        case WM_CLOSE: DestroyWindow(hwnd); break;
        case WM_DESTROY: PostQuitMessage(0); break;
        case WM_LBUTTONDOWN: hell_PushMouseDownEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_LEFT, ud->winId); break;
        case WM_RBUTTONDOWN: hell_PushMouseDownEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_RIGHT, ud->winId); break;
        case WM_MBUTTONDOWN: hell_PushMouseDownEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_MID, ud->winId); break;
        case WM_LBUTTONUP: hell_PushMouseUpEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_LEFT, ud->winId); break;
        case WM_RBUTTONUP: hell_PushMouseUpEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_RIGHT, ud->winId); break;
        case WM_MBUTTONUP: hell_PushMouseUpEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_MID, ud->winId); break;
        case WM_MOUSEMOVE: hell_PushMouseMotionEvent(ud->evqueue, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), HELL_MOUSE_LEFT, ud->winId); break;
        case WM_SIZE: hell_PushWindowResizeEvent(ud->evqueue, LOWORD(lParam), HIWORD(lParam), ud->winId); break;
        case WM_KEYDOWN: keyDown = true; break;
        case WM_KEYUP: keyDown = false; hell_PushKeyUpEvent(ud->evqueue, lastKey, ud->winId); break;
        case WM_CHAR: 
        {
            if (keyDown) // it could be that we only get WM_CHAR after a keydown... which would make this unnecesary
            {
                hell_PushKeyDownEvent(ud->evqueue, wParam, ud->winId);
                lastKey = wParam;
            }
            break;
        }
        default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

inline static int createWin32Window(Hell_EventQueue* queue, int width, int height, const char* name, Hell_Window* hellWindow)
{
    hellWindow->typeSpecificData = hell_Malloc(sizeof(Win32Window));
    Win32Window* win32Window = (Win32Window*)hellWindow->typeSpecificData;
    memset(win32Window, 0, sizeof(Win32Window));
    WNDCLASSEX wc;

    assert(winVars.instance);
    win32Window->hinstance = winVars.instance;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = win32Window->hinstance;
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
    win32Window->hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        MS_WINDOW_CLASS_NAME,
        _T("Title of my window"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,
        NULL, NULL, win32Window->hinstance, &win32Window->userdata);

    if(win32Window->hwnd == NULL)
    {
        MessageBox(NULL, _T("Window Creation Failed!"), _T("Error!"),
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(win32Window->hwnd, SW_SHOWNORMAL);
    UpdateWindow(win32Window->hwnd);

    hellWindow->type = HELL_WINDOW_WIN32_TYPE;
    hellWindow->width = width;
    hellWindow->height = height;

    return 0;

    // Step 3: The Message Loop
}

inline static void drainMsEventQueue(void)
{
    MSG Msg;
    while(PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if ( !GetMessage (&Msg, NULL, 0, 0) ) 
        {
            // X button hit.. i think. we quit.
            // hell_AddText("quit\n");
        } 
        // save the msg time, because wndprocs don't have access to the timestamp
        // g_wv.sysMsgTime = msg.time
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
}

inline static void destroyWin32Window(Hell_Window* window)
{

}

#endif
#endif