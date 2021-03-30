#include <assert.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <string.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_event.h>
#include <xcb/xproto.h>
#include <X11/keysym.h>
#include "display.h"
#include "common.h"
#include "evcodes.h"
#include "input.h"
#include "mem.h"
#include "xcb_window_type.h"

static_assert(__unix__, "Only support displays on unix currently");

static XcbWindow xcbWindow;

static char windowName[32] = "floating";
static xcb_key_symbols_t* pXcbKeySymbols;

static Hell_Window window;

static uint32_t getKeyCode(const xcb_key_press_event_t* event)
{
    // XCB documentation is fucking horrible. fucking last parameter is called col. wtf? 
    // no clue what that means. ZERO documentation on this function. trash.
    xcb_keysym_t keySym = xcb_key_symbols_get_keysym(pXcbKeySymbols, event->detail, 0); 
    uint32_t keyCode = 0;
    switch (keySym)
    {
        case XK_a:         keyCode = HELL_KEY_A; break;
        case XK_b:         keyCode = HELL_KEY_B; break;
        case XK_c:         keyCode = HELL_KEY_C; break;
        case XK_d:         keyCode = HELL_KEY_D; break;
        case XK_e:         keyCode = HELL_KEY_E; break;
        case XK_f:         keyCode = HELL_KEY_F; break;
        case XK_g:         keyCode = HELL_KEY_G; break;
        case XK_h:         keyCode = HELL_KEY_H; break;
        case XK_i:         keyCode = HELL_KEY_I; break;
        case XK_j:         keyCode = HELL_KEY_J; break;
        case XK_k:         keyCode = HELL_KEY_K; break;
        case XK_l:         keyCode = HELL_KEY_L; break;
        case XK_m:         keyCode = HELL_KEY_M; break;
        case XK_n:         keyCode = HELL_KEY_N; break;
        case XK_o:         keyCode = HELL_KEY_O; break;
        case XK_p:         keyCode = HELL_KEY_P; break;
        case XK_q:         keyCode = HELL_KEY_Q; break;
        case XK_r:         keyCode = HELL_KEY_R; break;
        case XK_s:         keyCode = HELL_KEY_S; break;
        case XK_t:         keyCode = HELL_KEY_T; break;
        case XK_u:         keyCode = HELL_KEY_U; break;
        case XK_v:         keyCode = HELL_KEY_V; break;
        case XK_w:         keyCode = HELL_KEY_W; break;
        case XK_x:         keyCode = HELL_KEY_X; break;
        case XK_y:         keyCode = HELL_KEY_Y; break;
        case XK_z:         keyCode = HELL_KEY_Z; break;
        case XK_1:         keyCode = HELL_KEY_1; break;
        case XK_2:         keyCode = HELL_KEY_2; break;
        case XK_3:         keyCode = HELL_KEY_3; break;
        case XK_4:         keyCode = HELL_KEY_4; break;
        case XK_5:         keyCode = HELL_KEY_5; break;
        case XK_6:         keyCode = HELL_KEY_6; break;
        case XK_7:         keyCode = HELL_KEY_7; break;
        case XK_8:         keyCode = HELL_KEY_8; break;
        case XK_9:         keyCode = HELL_KEY_9; break;
        case XK_space:     keyCode = HELL_KEY_SPACE; break;
        case XK_Control_L: keyCode = HELL_KEY_CTRL; break;
        case XK_Escape:    keyCode = HELL_KEY_ESC; break;
        default: keyCode = 0; break;
    }
    return keyCode;
}

static Hell_I_MouseData getMouseData(const xcb_generic_event_t* event)
{
    xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)event;
    Hell_I_MouseData mouseData;
    mouseData.x = motion->event_x;
    mouseData.y = motion->event_y;
    if (motion->detail == 1) mouseData.buttonCode = HELL_MOUSE_LEFT; 
    else if (motion->detail == 2) mouseData.buttonCode = HELL_MOUSE_MID; 
    else if (motion->detail == 3) mouseData.buttonCode = HELL_MOUSE_RIGHT;
    return mouseData;
}

static Hell_I_ResizeData getResizeData(const xcb_generic_event_t* event)
{
    xcb_resize_request_event_t* resize = (xcb_resize_request_event_t*)event;
    Hell_I_ResizeData data = {0};
    data.height = resize->height;
    data.width  = resize->width;
    return data;
}

static Hell_I_ResizeData getConfigureData(const xcb_generic_event_t* event)
{
    xcb_configure_notify_event_t* resize = (xcb_configure_notify_event_t*)event;
    Hell_I_ResizeData data = {0};
    data.height = resize->height;
    data.width  = resize->width;
    return data;
}

static void initXcbWindow(const uint16_t width, const uint16_t height, const char* name)
{
    if (name)
    {
        assert(strnlen(name, 32) < 32);
        strcpy(windowName, name);
    }
    window.width  = width;
    window.height = height;
    int screenNum = 0;
    xcbWindow.connection =     xcb_connect(NULL, &screenNum);
    xcbWindow.window     =     xcb_generate_id(xcbWindow.connection);

    const xcb_setup_t* setup   = xcb_get_setup(xcbWindow.connection);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);

    for (int i = 0; i < screenNum; i++)
    {
        xcb_screen_next(&iter);   
    }

    xcb_screen_t* screen = iter.data;

    uint32_t values[2];
    uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	values[0] = screen->black_pixel;
    //	we need to limit what events we are interested in.
    //	otherwise the queue will fill up with garbage
	values[1] = //XCB_EVENT_MASK_EXPOSURE |
		XCB_EVENT_MASK_POINTER_MOTION |
    //  XCB_EVENT_MASK_RESIZE_REDIRECT |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY |
	//	XCB_EVENT_MASK_ENTER_WINDOW |
		XCB_EVENT_MASK_KEY_PRESS |
        XCB_EVENT_MASK_KEY_RELEASE |
	//	XCB_EVENT_MASK_LEAVE_WINDOW |
		XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE;

    xcb_create_window(xcbWindow.connection, 
            XCB_COPY_FROM_PARENT,              // depth 
            xcbWindow.window,                  // window id
            screen->root,                      // parent
            0, 0,                              // x and y coordinate of new window
            width, height, 
            0,                                 // border wdith 
            XCB_WINDOW_CLASS_COPY_FROM_PARENT, // class 
            XCB_COPY_FROM_PARENT,              // visual 
            mask, values);                          // masks (TODO: set to get inputs)

    xcb_change_property(xcbWindow.connection, 
            XCB_PROP_MODE_REPLACE, 
            xcbWindow.window, 
            XCB_ATOM_WM_NAME, 
            XCB_ATOM_STRING, 8, strlen(windowName), windowName);

    xcb_map_window(xcbWindow.connection, xcbWindow.window);
    xcb_flush(xcbWindow.connection);
    pXcbKeySymbols = xcb_key_symbols_alloc(xcbWindow.connection);
    hell_Announce("Xcb Display initialized.\n");

    window.width            = width;
    window.height           = height;
    window.type             = HELL_WINDOW_XCB_TYPE;
    window.typeSpecificData = &xcbWindow;
}

static void drainXcbEventQueue(void)
{
    xcb_generic_event_t* xEvent = NULL;
    while ((xEvent = xcb_poll_for_event(xcbWindow.connection)))
    {
start:
        switch (XCB_EVENT_RESPONSE_TYPE(xEvent))
        {
            case XCB_KEY_PRESS: 
            {
                uint32_t keyCode = getKeyCode((xcb_key_press_event_t*)xEvent);
                if (keyCode != 0)
                    hell_i_PushKeyDownEvent(keyCode);
                break;
            }
            case XCB_KEY_RELEASE: 
            {
                // bunch of extra stuff here dedicated to detecting autrepeats
                // the idea is that if a key-release event is detected, followed
                // by an immediate keypress of the same key, its an autorepeat.
                // its unclear to me whether very rapidly hitting a key could
                // result in the same thing, and wheter it is worthwhile 
                // accounting for that
                uint32_t keyCode = getKeyCode((xcb_key_press_event_t*)xEvent);
                if (keyCode == 0) break;
                // need to see if this is actually an auto repeat
                xcb_generic_event_t* next = xcb_poll_for_event(xcbWindow.connection);
                if (next) 
                {
                    uint8_t type = XCB_EVENT_RESPONSE_TYPE(next);
                    uint32_t keyCodeNext = getKeyCode((xcb_key_press_event_t*)next);
                    // if next is not a press or the key code is different then neither are autorepeats
                    if (type != XCB_KEY_PRESS || keyCode != keyCodeNext)
                    {
                        hell_i_PushKeyUpEvent(keyCode);
                        free(xEvent);
                        xEvent = next;
                        goto start;
                    }
                    else // is an autorepeat
                        free(next);
                    break;
                }
                hell_i_PushKeyUpEvent(keyCode);
                break;
            }
            case XCB_BUTTON_PRESS:
            {
                Hell_I_MouseData data = getMouseData(xEvent);
                hell_i_PushMouseDownEvent(data.x, data.y, data.buttonCode);
                break;
            }
            case XCB_BUTTON_RELEASE:
            {
                Hell_I_MouseData data = getMouseData(xEvent);
                hell_i_PushMouseUpEvent(data.x, data.y, data.buttonCode);
                break;
            }
            case XCB_MOTION_NOTIFY:
            {
                Hell_I_MouseData data = getMouseData(xEvent);
                hell_i_PushMouseMotionEvent(data.x, data.y, data.buttonCode);
                break;
            }
            case XCB_RESIZE_REQUEST:
            {
                Hell_I_ResizeData data = getResizeData(xEvent);
                if (data.width == window.width && data.height == window.height)
                    break;
                window.width = data.width;
                window.height = data.height;
                hell_i_PushWindowResizeEvent(data.width, data.height);
                break;
            }
            // for some reason resize events seem to come through here.... but so do window moves...
            // TODO: throw out window moves.
            case XCB_CONFIGURE_NOTIFY: 
            {
                Hell_I_ResizeData data = getConfigureData(xEvent);
                if (data.width == window.width && data.height == window.height)
                    break;
                window.width = data.width;
                window.height = data.height;
                hell_i_PushWindowResizeEvent(data.width, data.height);
                break;
            }
            default: break;
        }
        free(xEvent); // using clib free directly because we did not allocate the xevent
    }
}

static void cleanUpXcb(void)
{
    xcb_key_symbols_free(pXcbKeySymbols);
    xcb_flush(xcbWindow.connection);
    xcb_destroy_window(xcbWindow.connection, xcbWindow.window);
    xcb_disconnect(xcbWindow.connection);
}

const Hell_Window* hell_d_Init(const uint16_t width, const uint16_t height, const char* name)
{
    // once we support other platforms we can put a switch in here
    initXcbWindow(width, height, name);
    return &window;
}

void hell_d_DrainWindowEvents(void)
{
    if (window.typeSpecificData) // used to it is active
        drainXcbEventQueue();
}

void hell_d_CleanUp(void)
{
    cleanUpXcb();
    hell_Announce("Display shutdown.\n");
}
