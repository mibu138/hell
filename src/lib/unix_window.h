#ifndef HELL_UNIX_WINDOW_H
#define HELL_UNIX_WINDOW_H

#include "platform.h"
#ifdef UNIX
#include "xcb_window_type.h"
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb_event.h>
#include <xcb/xproto.h>
#include <X11/keysym.h>
#include <xcb/xcb.h>
#include <xcb/xinput.h>
#include "evcodes.h"
#include "input.h"
#include "private.h"
#include "window.h"
#include "common.h"
#include "debug.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#ifndef UNIX
#error "Must be on a unix platform to use xcb windows"
#endif

#define MAX_WIN_NAME 32

#ifdef HELL_XCB_DEBUG_MESSAGES 
#define DPRINT(fmt, ...) hell_DebugPrint("xcb", fmt, ##__VA_ARGS__)
#else 
#define DPRINT(fmt, ...) (void)0 
#endif

typedef xcb_input_button_press_event_t input_device_event_t;

typedef struct Hell_XcbWindow {
    xcb_connection_t*  connection;
    xcb_key_symbols_t* keysymbols;
    xcb_window_t       window;
    char               name[MAX_WIN_NAME];
} Hell_XcbWindow;

inline static uint32_t getXcbKeyCode(xcb_key_symbols_t* keysymbols, const xcb_key_press_event_t* event)
{
    // XCB documentation is fucking horrible. fucking last parameter is called col. wtf? 
    // no clue what that means. ZERO documentation on this function. trash.
    xcb_keysym_t keySym = xcb_key_symbols_get_keysym(keysymbols, event->detail, 0); 
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


inline static int fixed1616ToInt(xcb_input_fp1616_t i)
{
    return (int)((float)i / 0x10000);
}

inline static Hell_MouseEventData getXcbMouseData(const xcb_generic_event_t* event)
{
    xcb_motion_notify_event_t* motion = (xcb_motion_notify_event_t*)event;
    Hell_MouseEventData mouseData;
    mouseData.x = motion->event_x;
    mouseData.y = motion->event_y;
    if (motion->detail == 1) mouseData.buttonCode = HELL_MOUSE_LEFT; 
    else if (motion->detail == 2) mouseData.buttonCode = HELL_MOUSE_MID; 
    else if (motion->detail == 3) mouseData.buttonCode = HELL_MOUSE_RIGHT;
    return mouseData;
}

inline static Hell_MouseEventData getXInputMouseData(const input_device_event_t* event)
{
    Hell_MouseEventData mouseData;
    mouseData.x = fixed1616ToInt(event->event_x);
    mouseData.y = fixed1616ToInt(event->event_y);
    if (event->detail == 1) mouseData.buttonCode = HELL_MOUSE_LEFT; 
    else if (event->detail == 2) mouseData.buttonCode = HELL_MOUSE_MID; 
    else if (event->detail == 3) mouseData.buttonCode = HELL_MOUSE_RIGHT;
    return mouseData;
}

inline static Hell_ResizeEventData getXcbResizeData(const xcb_generic_event_t* event)
{
    xcb_resize_request_event_t* resize = (xcb_resize_request_event_t*)event;
    Hell_ResizeEventData data = {0};
    data.height = resize->height;
    data.width  = resize->width;
    return data;
}

inline static Hell_ResizeEventData getXcbConfigureData(const xcb_generic_event_t* event)
{
    xcb_configure_notify_event_t* resize = (xcb_configure_notify_event_t*)event;
    Hell_ResizeEventData data = {0};
    data.height = resize->height;
    data.width  = resize->width;
    return data;
}

static double 
fixed3232ToDouble(xcb_input_fp3232_t input)
{
    return (double)input.integral + (double)input.frac / (1ULL << 32);
}

static int        stylusDeviceId = -1;
static xcb_atom_t pressureAtom;

static struct StylusPressureInfo {
    xcb_atom_t label;
    double     min;
    double     max;
    int        number;
} stylusPressureInfo;

static void 
setUpDevices(Hell_XcbWindow* w)
{
    xcb_input_xi_query_device_cookie_t cookie = xcb_input_xi_query_device(w->connection, XCB_INPUT_DEVICE_ALL);
    xcb_input_xi_query_device_reply_t* reply = xcb_input_xi_query_device_reply(w->connection, cookie, NULL);
    assert(reply);
    xcb_input_xi_device_info_iterator_t iter = xcb_input_xi_query_device_infos_iterator(reply);
    while (iter.rem)
    {
        xcb_input_xi_device_info_t* deviceInfo = iter.data;
        const char* name = xcb_input_xi_device_info_name(deviceInfo);
        switch (deviceInfo->type)
        {
            case XCB_INPUT_DEVICE_TYPE_MASTER_KEYBOARD: 
                hell_Print("Found master keyboard: deviceId: %d name: %s\n", deviceInfo->deviceid, name); 
                break;
            case XCB_INPUT_DEVICE_TYPE_MASTER_POINTER: 
                hell_Print("Found master pointer: deviceId: %d name: %s\n", deviceInfo->deviceid, name); 
                break;
            case XCB_INPUT_DEVICE_TYPE_SLAVE_KEYBOARD: 
                hell_Print("Found slave keyboard: deviceId: %d name: %s\n", deviceInfo->deviceid, name); 
                break;
            case XCB_INPUT_DEVICE_TYPE_SLAVE_POINTER: 
                {
                hell_Print("Found slave pointer: deviceId: %d name: %s\n", deviceInfo->deviceid, name); 
                bool isStylus = (strcmp(name, "Wacom Intuos Pro S Pen stylus") == 0); //TODO figure out a better way to check
                if (isStylus) stylusDeviceId = deviceInfo->deviceid;
                xcb_input_device_class_iterator_t ci = xcb_input_xi_device_info_classes_iterator(deviceInfo);
                while (ci.rem)
                {
                    xcb_input_device_class_t* classInfo = ci.data;
                    switch (classInfo->type)
                    {
                        case XCB_INPUT_DEVICE_CLASS_TYPE_VALUATOR: 
                        {
                            xcb_input_valuator_class_t* vci = (xcb_input_valuator_class_t*)classInfo;
                            hell_Print("VCI Label: %d\n", vci->label);
                            xcb_get_atom_name_cookie_t atcookie = xcb_get_atom_name(w->connection, vci->label);
                            xcb_get_atom_name_reply_t* atreply = xcb_get_atom_name_reply(w->connection, atcookie, NULL);
                            if (atreply)
                            {
                                const char* atomname = xcb_get_atom_name_name(atreply);
                                hell_Print("VCI Name: %s\n", atomname);
                            }
                            if (isStylus && vci->label == pressureAtom)
                            {
                                stylusPressureInfo.label  = vci->label;
                                stylusPressureInfo.min    = fixed3232ToDouble(vci->min);
                                stylusPressureInfo.max    = fixed3232ToDouble(vci->max);
                                stylusPressureInfo.number = vci->number;
                                hell_Print("Stylus info: \n\tlabel: %d\n\tmin: %f\n\tmax: %f\n\tnumber: %d\n", 
                                        stylusPressureInfo.label, stylusPressureInfo.min, stylusPressureInfo.max, stylusPressureInfo.number);
                            }
                            free(atreply);
                            break;
                        }
                    }
                    xcb_input_device_class_next(&ci);
                }
                break;
                }
        }
        xcb_input_xi_device_info_next(&iter);
    }
    free(reply);
}

struct fuck_xcb {
    xcb_input_event_mask_t header;
    uint32_t               mask;
};

inline static void createXcbWindow(const uint16_t width, const uint16_t height, const char* name, Hell_Window* window)
{
    window->typeSpecificData = hell_Malloc(sizeof(Hell_XcbWindow));
    Hell_XcbWindow* xcbWindow = (Hell_XcbWindow*)window->typeSpecificData;
    memset(xcbWindow, 0, sizeof(Hell_XcbWindow));
    if (name)
    {
        assert(strnlen(name, MAX_WIN_NAME) < MAX_WIN_NAME);
        strncpy(xcbWindow->name, name, MAX_WIN_NAME);
    }
    else 
    {
        strcpy(xcbWindow->name, "floating");
    }
    window->width  = width;
    window->height = height;
    int screenNum = 0;
    xcbWindow->connection =     xcb_connect(NULL, &screenNum);
    xcbWindow->window     =     xcb_generate_id(xcbWindow->connection);

    const xcb_setup_t* setup   = xcb_get_setup(xcbWindow->connection);
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
		XCB_EVENT_MASK_BUTTON_RELEASE |
        XCB_EVENT_MASK_PROPERTY_CHANGE;

    xcb_create_window(xcbWindow->connection, 
            XCB_COPY_FROM_PARENT,              // depth 
            xcbWindow->window,                  // window id
            screen->root,                      // parent
            0, 0,                              // x and y coordinate of new window
            width, height, 
            0,                                 // border wdith 
            XCB_WINDOW_CLASS_COPY_FROM_PARENT, // class 
            XCB_COPY_FROM_PARENT,              // visual 
            mask, values);                          // masks (TODO: set to get inputs)

    xcb_change_property(xcbWindow->connection, 
            XCB_PROP_MODE_REPLACE, 
            xcbWindow->window, 
            XCB_ATOM_WM_NAME, 
            XCB_ATOM_STRING, 8, strlen(xcbWindow->name), xcbWindow->name);

    uint32_t xiBitMask = 
        XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
        XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE |
        XCB_INPUT_XI_EVENT_MASK_MOTION;

    struct fuck_xcb xiMask = {0};
    xiMask.header.deviceid = XCB_INPUT_DEVICE_ALL;
    xiMask.header.mask_len = 1;
    xiMask.mask = xiBitMask;

    // XCB has the absolute worst fucking API I've ever seen. Nothing is documented.
    // Can only figure out how to use it by poring over the one or two projects in the wild 
    // that actually bothered to write code for this garbage, and those projects must have 
    // gotten some kind of support from the XCB devs because their headers ARE FUCKING INPENETRABLE. SHOVE YOUR TODOS UP YOUR ASS.
    // Its a fucking shit show and should be burned off the face of the earth.
    // Can't wait for wayland to take over and to never again deal with this fucking cunt of an API again.
    xcb_void_cookie_t cookieA = xcb_input_xi_select_events_checked(xcbWindow->connection, xcbWindow->window, 1, &xiMask.header);
    xcb_generic_error_t* error = xcb_request_check(xcbWindow->connection, cookieA);
    if (error)
    {
        hell_Print("XCB ERROR\n");
        exit(0);
    }

    const char* pressureName = "Abs Pressure";
    xcb_intern_atom_reply_t* reply;
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(xcbWindow->connection, 1, strlen(pressureName), pressureName);
    reply = xcb_intern_atom_reply(xcbWindow->connection, cookie, NULL);
    DPRINT("Pressure Atom: %d\n", reply->atom);
    pressureAtom = reply->atom;

    xcb_map_window(xcbWindow->connection, xcbWindow->window);
    xcb_flush(xcbWindow->connection);
    xcbWindow->keysymbols = xcb_key_symbols_alloc(xcbWindow->connection);

    window->width            = width;
    window->height           = height;
    window->type             = HELL_WINDOW_XCB_TYPE;

    free(reply);
    setUpDevices(xcbWindow);
}

static int xi2ValuatorOffset(const unsigned char *maskPtr, int maskLen, int number)
{
    int offset = 0;
    for (int i = 0; i < maskLen; i++) {
        if (number < 8) {
            if ((maskPtr[i] & (1 << number)) == 0)
                return -1;
        }
        for (int j = 0; j < 8; j++) {
            if (j == number)
                return offset;
            if (maskPtr[i] & (1 << j))
                offset++;
        }
        number -= 8;
    }
    return -1;
}

// "inspired" from qt because the xcb devs don't document fucking shit. this is insane and 
// i haven't the foggiest fuck what is going on.
// TODO re-write this once I learn what fuck is happening here.
inline static bool whatTheFuck(const void* event, int valuatorNum, double *value)
{
    const input_device_event_t* xideviceevent = (const input_device_event_t *)event;
    const unsigned char* buttonsMaskAddr = (const unsigned char *)(&xideviceevent[1]);
    const unsigned char *valuatorsMaskAddr = buttonsMaskAddr + xideviceevent->buttons_len * 4;
    const xcb_input_fp3232_t* valuatorsValuesAddr = (const xcb_input_fp3232_t *)(valuatorsMaskAddr + xideviceevent->valuators_len * 4);

    int valuatorOffset = xi2ValuatorOffset(valuatorsMaskAddr, xideviceevent->valuators_len, valuatorNum);
    if (valuatorOffset < 0)
        return false;

    *value = valuatorsValuesAddr[valuatorOffset].integral;
    *value += ((double)valuatorsValuesAddr[valuatorOffset].frac / (1 << 16) / (1 << 16));
    return true;
}

inline static void handleStylusEvent(Hell_EventQueue* queue, Hell_Window* window, input_device_event_t* event)
{
    DPRINT("Got event. Response type: %d\n", event->response_type);
    double pressure = 0.0;
    bool r = whatTheFuck(event, stylusPressureInfo.number, &pressure);
    float normalizedPressure = pressure / stylusPressureInfo.max;
    hell_PushStylusEvent(queue, normalizedPressure, window->id);
    DPRINT("Result: %d Pressure: %f\n", r, normalizedPressure);
}

inline static void handleXInputEvent(Hell_EventQueue* queue, Hell_Window* window, input_device_event_t* event)
{
    DPRINT("Event device ID: %d\n", event->deviceid);
    DPRINT("Event source ID: %d\n", event->sourceid);
    DPRINT("Event type: %d\n", event->event_type);
    switch (event->event_type)
    {
    case XCB_BUTTON_PRESS:
        DPRINT("button press\n");
        Hell_MouseEventData data = getXInputMouseData(event);
        DPRINT("Mouse event data:\n\t x: %d y: %d button: %d\n", data.x, data.y, data.buttonCode);
        hell_PushMouseDownEvent(queue, data.x, data.y, data.buttonCode, window->id);
        break;
    case XCB_BUTTON_RELEASE:
        DPRINT("button release\n");
        data = getXInputMouseData(event);
        DPRINT("Mouse event data:\n\t x: %d y: %d, button: %d\n", data.x, data.y, data.buttonCode);
        hell_PushMouseUpEvent(queue, data.x, data.y, data.buttonCode, window->id);
        break;
    case XCB_MOTION_NOTIFY:
        DPRINT("motion notify\n");
        data = getXInputMouseData(event);
        DPRINT("Mouse event data:\n\t x: %d y: %d\n", data.x, data.y);
        hell_PushMouseMotionEvent(queue, data.x, data.y, data.buttonCode, window->id);
        break;
    default: 
        break;
    }
    if (event->sourceid == stylusDeviceId)
    {
        handleStylusEvent(queue, window, event);
    }
}

inline static void drainXcbEventQueue(Hell_EventQueue* queue, Hell_Window* window)
{
    assert(queue);
    assert(window);
    Hell_XcbWindow* xcbWindow = (Hell_XcbWindow*)window->typeSpecificData;
    xcb_generic_event_t* xEvent = NULL;
    while ((xEvent = xcb_poll_for_event(xcbWindow->connection)))
    {
start:
        DPRINT("Response type: %d\n", xEvent->response_type);
        switch (XCB_EVENT_RESPONSE_TYPE(xEvent))
        {
            case XCB_KEY_PRESS: 
            {
                DPRINT("key press\n");
                uint32_t keyCode = getXcbKeyCode(xcbWindow->keysymbols, (xcb_key_press_event_t*)xEvent);
                if (keyCode != 0)
                    hell_PushKeyDownEvent(queue, keyCode, window->id);
                break;
            }
            case XCB_KEY_RELEASE: 
            {
                DPRINT("key release %d\n", 5);
                // bunch of extra stuff here dedicated to detecting autrepeats
                // the idea is that if a key-release event is detected, followed
                // by an immediate keypress of the same key, its an autorepeat.
                // its unclear to me whether very rapidly hitting a key could
                // result in the same thing, and wheter it is worthwhile 
                // accounting for that
                uint32_t keyCode = getXcbKeyCode(xcbWindow->keysymbols, (xcb_key_press_event_t*)xEvent);
                if (keyCode == 0) break;
                // need to see if this is actually an auto repeat
                xcb_generic_event_t* next = xcb_poll_for_event(xcbWindow->connection);
                if (next) 
                {
                    uint8_t type = XCB_EVENT_RESPONSE_TYPE(next);
                    uint32_t keyCodeNext = getXcbKeyCode(xcbWindow->keysymbols, (xcb_key_press_event_t*)next);
                    // if next is not a press or the key code is different then neither are autorepeats
                    if (type != XCB_KEY_PRESS || keyCode != keyCodeNext)
                    {
                        hell_PushKeyUpEvent(queue, keyCode, window->id);
                        free(xEvent);
                        xEvent = next;
                        goto start;
                    }
                    else // is an autorepeat
                        free(next);
                    break;
                }
                hell_PushKeyUpEvent(queue, keyCode, window->id);
                break;
            }
            case XCB_BUTTON_PRESS:
            {
                DPRINT("button press\n");
                Hell_MouseEventData data = getXcbMouseData(xEvent);
                hell_PushMouseDownEvent(queue, data.x, data.y, data.buttonCode, window->id);
                break;
            }
            case XCB_BUTTON_RELEASE:
            {
                DPRINT("button release\n");
                Hell_MouseEventData data = getXcbMouseData(xEvent);
                hell_PushMouseUpEvent(queue, data.x, data.y, data.buttonCode, window->id);
                break;
            }
            case XCB_MOTION_NOTIFY:
            {
                Hell_MouseEventData data = getXcbMouseData(xEvent);
                hell_PushMouseMotionEvent(queue, data.x, data.y, data.buttonCode, window->id);
                break;
            }
            case XCB_RESIZE_REQUEST:
            {
                Hell_ResizeEventData data = getXcbResizeData(xEvent);
                if (data.width == window->width && data.height == window->height)
                    break;
                window->width = data.width;
                window->height = data.height;
                hell_PushWindowResizeEvent(queue, data.width, data.height, window->id);
                break;
            }
            // for some reason resize events seem to come through here.... but so do window moves...
            // TODO: throw out window moves.
            case XCB_CONFIGURE_NOTIFY: 
            {
                Hell_ResizeEventData data = getXcbConfigureData(xEvent);
                if (data.width == window->width && data.height == window->height)
                    break;
                window->width = data.width;
                window->height = data.height;
                hell_PushWindowResizeEvent(queue, data.width, data.height, window->id);
                break;
            }
            case XCB_GE_GENERIC:
            {
                DPRINT("Generic\n");
                handleXInputEvent(queue, window, (input_device_event_t*)xEvent);
                break;
            }
            default: break;
        }
        free(xEvent); // using clib free directly because we did not allocate the xevent
    }
}

inline static void destroyXcbWindow(Hell_Window* window)
{
    assert(window->type == HELL_WINDOW_XCB_TYPE);
    Hell_XcbWindow* xcbWindow = (Hell_XcbWindow*)window->typeSpecificData;
    xcb_key_symbols_free(xcbWindow->keysymbols);
    xcb_flush(xcbWindow->connection);
    xcb_destroy_window(xcbWindow->connection, xcbWindow->window);
    xcb_disconnect(xcbWindow->connection);
    hell_Free(xcbWindow);
    memset(xcbWindow, 0, sizeof(Hell_XcbWindow));
}


#endif
#endif
