#ifndef HELL_INPUT_H
#define HELL_INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int16_t  x;
    int16_t  y;
    uint8_t  buttonCode;
} Hell_I_MouseData;

typedef struct {
    uint16_t width;
    uint16_t height;
} Hell_I_ResizeData;

typedef union {
    uint32_t          keyCode;
    Hell_I_MouseData  mouseData;
    Hell_I_ResizeData resizeData;
} Hell_I_EventData;

typedef enum {
    HELL_I_NONE,
    HELL_I_KEYDOWN,
    HELL_I_KEYUP,
    HELL_I_MOUSEDOWN,
    HELL_I_MOUSEUP,
    HELL_I_MOTION,
    HELL_I_RESIZE,
    HELL_I_CONSOLE
} Hell_I_EventType;

typedef enum {
    HELL_I_NONE_BIT   = 1 << 0,
    HELL_I_KEY_BIT    = 1 << 1,
    HELL_I_MOUSE_BIT  = 1 << 2,
    HELL_I_WINDOW_BIT = 1 << 3,
    HELL_I_ALL_BIT    = ~(uint32_t)1
} Hell_I_EventMaskBits;

typedef uint32_t Hell_I_EventMask;

typedef struct Hell_I_Event {
    Hell_I_EventType type;
    Hell_I_EventData data;
    Hell_I_EventMask mask;
    uint64_t         time;
    uint64_t         ptrLen;
    void*            ptr;
} Hell_I_Event;

typedef bool (*Hell_I_SubscriberFn)(const Hell_I_Event*);

void hell_i_Init(void);
void hell_i_PumpEvents(void);
void hell_i_DrainEvents(void);
void hell_i_Subscribe(Hell_I_SubscriberFn func, Hell_I_EventMask mask);

void hell_i_PushWindowResizeEvent(unsigned int width, unsigned int height);
void hell_i_PushMouseDownEvent(int16_t x, int16_t y, uint8_t buttonCode);
void hell_i_PushMouseUpEvent(int16_t x, int16_t y, uint8_t buttonCode);
void hell_i_PushMouseMotionEvent(int16_t x, int16_t y, uint8_t buttonCode);
void hell_i_PushKeyDownEvent(uint32_t keyCode);
void hell_i_PushKeyUpEvent(uint32_t keyCode);
void hell_i_PushEmptyEvent(void);

#endif /* end of include guard: HELL_INPUT_H */
