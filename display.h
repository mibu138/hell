#ifndef HELL_DISPLAY_H
#define HELL_DISPLAY_H

typedef enum {
    HELL_WINDOW_XCB_TYPE
} Hell_WindowType;

typedef struct {
    unsigned int    width;
    unsigned int    height;
    Hell_WindowType type;
    void*           typeSpecificData;
} Hell_Window;

const Hell_Window* hell_d_Init(const unsigned short width, const unsigned short height, const char* name);
void               hell_d_CleanUp(void);
// does nothing if window is not active
void               hell_d_DrainWindowEvents(void);

#endif /* end of include guard: DISPLAY_H */
