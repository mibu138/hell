#ifndef HELL_XCB_WINDOW_TYPE_H
#define HELL_XCB_WINDOW_TYPE_H

#include <xcb/xcb.h>

typedef struct {
    xcb_connection_t* connection;
    xcb_window_t      window;
} XcbWindow;

#endif /* end of include guard: HELL_XCB_WINDOW_TYPE_H */
