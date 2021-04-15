#ifndef HELL_WINDOW_H
#define HELL_WINDOW_H

typedef enum {
    HELL_WINDOW_XCB_TYPE,
    HELL_WINDOW_WIN32_TYPE
} Hell_WindowType;

typedef struct Hell_Window {
    unsigned int    width;
    unsigned int    height;
    Hell_WindowType type;
    void*           typeSpecificData;
} Hell_Window;

const Hell_Window* hell_w_Init(const unsigned short width, const unsigned short height, const char* name);
void               hell_w_CleanUp(void);
// does nothing if window is not active
void               hell_w_DrainWindowEvents(void);

#endif /* end of include guard: DISPLAY_H */
