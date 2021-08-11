#ifndef HELL_INPUT_H
#define HELL_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"
#include "evcodes.h"

typedef struct {
    int16_t       x;
    int16_t       y;
    uint8_t       buttonCode;
} Hell_MouseEventData;

typedef struct {
    uint16_t      width;
    uint16_t      height;
} Hell_ResizeEventData;

typedef struct {
    void*            ptr;
    uint32_t         ptrLen;
} Hell_ConsoleEventData;

typedef struct {
    uint32_t      keyCode;
} Hell_KeyEventData;

typedef union {
    Hell_MouseEventData   mouseData;
    Hell_ResizeEventData  resizeData;
    Hell_KeyEventData     keyData;
} Hell_InputData;

// data that is captured via a window
typedef struct {
    Hell_InputData data;
    Hell_WindowID  windowID;
} Hell_WindowEventData;

// data that is captured via a device directly
typedef struct {
    Hell_InputData data;
} Hell_DeviceEventData;

typedef union {
    Hell_WindowEventData  winData;
    Hell_DeviceEventData  devData;
    Hell_ConsoleEventData conData;
} Hell_EventData;

typedef enum {
    HELL_EVENT_TYPE_NONE,
    HELL_EVENT_TYPE_KEYDOWN,
    HELL_EVENT_TYPE_KEYUP,
    HELL_EVENT_TYPE_MOUSEDOWN,
    HELL_EVENT_TYPE_MOUSEUP,
    HELL_EVENT_TYPE_MOTION,
    HELL_EVENT_TYPE_RESIZE,
    HELL_EVENT_TYPE_CONSOLE
} Hell_EventType;

typedef enum {
    HELL_EVENT_MASK_NONE_BIT    = 1 << 0,
    HELL_EVENT_MASK_KEY_BIT     = 1 << 1,
    HELL_EVENT_MASK_MOUSE_BIT   = 1 << 2,
    HELL_EVENT_MASK_WINDOW_BIT  = 1 << 3,
    HELL_EVENT_MASK_CONSOLE_BIT = 1 << 4,
    HELL_EVENT_MASK_DEVICE_BIT  = 1 << 5,
    HELL_EVENT_MASK_ALL_BIT     = ~(uint32_t)1
} Hell_EventMaskBits;

typedef uint32_t Hell_EventMask;

typedef struct Hell_Event {
    Hell_EventData data;
    Hell_EventType type;
    Hell_EventMask mask;
    uint64_t       time;
} Hell_Event;

typedef struct Hell_EventQueue Hell_EventQueue;
typedef struct Hell_Console    Hell_Console;
typedef struct Hell_Window     Hell_Window;

// returning true consumes event
typedef bool (*Hell_SubscriberFn)(const Hell_Event*, void* data);

// starts the clock for all hell instances.
// safe to call even if clock is already started.
void hell_StartClock(void);
bool hell_ClockStarted(void);

// generates events from input sources and moves them into the event queue.
// neither window nor console are required. if they are null it just does not
// try to generate events from them.
void hell_CoagulateInput(Hell_EventQueue* queue, Hell_Console* console, uint32_t windowCount, Hell_Window* windows[]);

// drains the event queue and calls subscriber functions
void hell_SolveInput(Hell_EventQueue* queue);

void hell_Subscribe(Hell_EventQueue* queue, Hell_EventMask mask, Hell_WindowID winid, Hell_SubscriberFn func,
                           void* data);
void hell_Unsubscribe(Hell_EventQueue*, const Hell_SubscriberFn);

void hell_CreateConsole(Hell_Console*);
void hell_CreateEventQueue(Hell_EventQueue*);
void hell_DestroyConsole(Hell_Console*);
void hell_DestroyEventQueue(Hell_EventQueue*);

void hell_PushWindowResizeEvent(Hell_EventQueue*, uint32_t width,
                                uint32_t height, Hell_WindowID winid);
void hell_PushMouseDownEvent(Hell_EventQueue*, int16_t x, int16_t y,
                             uint8_t buttonCode, Hell_WindowID winid);
void hell_PushMouseUpEvent(Hell_EventQueue*, int16_t x, int16_t y,
                           uint8_t buttonCode, Hell_WindowID winid);
void hell_PushMouseMotionEvent(Hell_EventQueue*, int16_t x, int16_t y,
                               uint8_t buttonCode, Hell_WindowID winid);
void hell_PushKeyDownEvent(Hell_EventQueue*, uint32_t keyCode, Hell_WindowID winid);
void hell_PushKeyUpEvent(Hell_EventQueue*, uint32_t keyCode, Hell_WindowID winid);
void hell_PushEmptyEvent(Hell_EventQueue*);

uint16_t hell_GetWindowResizeWidth(const Hell_Event* event);
uint16_t hell_GetWindowResizeHeight(const Hell_Event* event);
int16_t  hell_GetMouseX(const Hell_Event*);
int16_t  hell_GetMouseY(const Hell_Event*);

uint8_t hell_GetEventButtonCode(const Hell_Event* event);
uint8_t hell_GetEventKeyCode(const Hell_Event* event);

#endif /* end of include guard: HELL_INPUT_H */
