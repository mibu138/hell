#ifndef HELL_XCB_WINDOW_TYPE_H
#define HELL_XCB_WINDOW_TYPE_H

typedef struct {
    xcb_connection_t* connection;
    xcb_window_t      window;
} XcbWindow;

#endif /* end of include guard: HELL_XCB_WINDOW_TYPE_H */
