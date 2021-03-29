#include <assert.h>
#include <xcb/xcb.h>
#include <string.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xproto.h>
#include "common.h"

static_assert(__unix__, "Only support displays on unix currently");

typedef struct {
    xcb_connection_t* connection;
    xcb_window_t      window;
} XcbWindow;

static XcbWindow xcbWindow;

static uint32_t windowWidth;
static uint32_t windowHeight;

static char windowName[32] = "floating";
static xcb_key_symbols_t* pXcbKeySymbols;

static void initXcbWindow(const uint16_t width, const uint16_t height, const char* name)
{
    if (name)
    {
        assert(strnlen(name, 32) < 32);
        strcpy(windowName, name);
    }
    windowWidth  = width;
    windowHeight = height;
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
    hell_Print("Xcb Display initialized.\n");
}

void hell_d_Init(const uint16_t width, const uint16_t height, const char* name)
{
    // once we support other platforms we can put a switch in here
    initXcbWindow(width, height, name);
}

