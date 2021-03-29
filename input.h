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

typedef struct Hell_I_Event {
    Hell_I_EventType type;
    Hell_I_EventData data;
    uint64_t         time;
    uint64_t         ptrLen;
    void*            ptr;
} Hell_I_Event;

typedef bool (*Hell_I_SubscriberFn)(const Hell_I_Event*);

void hell_i_Init(void);
void hell_i_PumpEvents(void);
void hell_i_DrainEvents(void);
void hell_i_PushEvent(Hell_I_Event event);

#endif /* end of include guard: HELL_INPUT_H */
