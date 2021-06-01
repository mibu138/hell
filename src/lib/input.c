#include "input.h"
#include "cmd.h"
#include "common.h"
#include "debug.h"
#include "platform.h"
#include "private.h"
#include "window.h"
#include <assert.h>
#include <fcntl.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
//
#ifdef UNIX
#include <termios.h>
// stash the tc at program start here so we can reset it on shut down
struct termios origTc;
#elif defined(WINDOWS)
#include <windows.h>
#endif

static_assert(sizeof(Hell_EventType) == 4,
              "sizeof(Hell_I_EventType) should be 4");
static_assert(sizeof(Hell_EventMask) == 4,
              "sizeof(Hell_I_EventMask) should be 4");
static_assert(
    sizeof(Hell_Event) == 32,
    "sizeof(Hell_I_Event) should be 32, to allow 2 events to be read with each "
    "cacheline read (assuming the event queue is aligned well)");

typedef struct {
    int  cursor;
    int  scroll;
    int  widthInChars;
    char buffer[MAX_EDIT_LINE];
} Field;

// returning true consumes the event
#define MAX_SUBSCRIBERS 32

typedef struct {
    void*               data;
    Hell_SubscriberFn func;
    Hell_EventMask    eventMask;
} Hell_Subscription;

//
// bk000306: upped this from 64
#define MAX_QUEUE_EVENTS 64
#define MASK_QUEUE_EVENTS (MAX_QUED_EVENTS - 1)

typedef struct Hell_Console {
    int   eraseCode;
    int   eofCode;
    Field console;
} Hell_Console;

typedef struct Hell_EventQueue {
    Hell_Subscription subscriptions[MAX_SUBSCRIBERS];
    int                 subscriberCount;
    Hell_Event        queue[MAX_QUEUE_EVENTS];
    // bk000306: initialize
    int                 head;
    int                 tail;
} Hell_EventQueue;

static_assert(STDIN_FILENO == 0, "We assume the the fd for stdin is 0");

static int  globalConsoleCounter;
static bool globalClockStarted;

#ifdef UNIX
static struct timespec unixEpoch;
#define UNIX_CLOCK_ID CLOCK_MONOTONIC

static void
initUnixTime(void)
{
    clock_gettime(UNIX_CLOCK_ID, &unixEpoch);
}

static uint64_t
getUnixMicroSeconds(void)
{
    struct timespec curTime;
    clock_gettime(UNIX_CLOCK_ID, &curTime);
    time_t   s  = curTime.tv_sec - unixEpoch.tv_sec;
    long     ns = curTime.tv_nsec - unixEpoch.tv_nsec;
    uint64_t ms = s * 1000000 + ns / 1000;
    return ms;
}

#elif defined(WINDOWS)

static LARGE_INTEGER winEpoch;
static LARGE_INTEGER winFreq;

static void
initWinTime(void)
{
    QueryPerformanceFrequency(&winFreq);
    QueryPerformanceCounter(&winEpoch);
}

static uint64_t
getWinMicroSeconds(void)
{
    LARGE_INTEGER curTicks, elapsedTime;
    QueryPerformanceCounter(&curTicks);
    elapsedTime.QuadPart = curTicks.QuadPart - winEpoch.QuadPart;
    elapsedTime.QuadPart *= 1000000;
    assert(winFreq.QuadPart > 0);
    elapsedTime.QuadPart /= winFreq.QuadPart;
    return elapsedTime.QuadPart;
}

#else
#error
#endif

static void
initTime(void)
{
    if (globalClockStarted)
        return;
#ifdef UNIX
    initUnixTime();
#elif defined(WINDOWS)
    initWinTime();
    globalClockStarted = true;
#else
#error
#endif
}

static void
fieldClear(Field* field)
{
    memset(field, 0, sizeof(Field));
}
// do a backspace
// TTimo NOTE: it seems on some terminals just sending '\b' is not enough
//   so for now, in any case we send "\b \b" .. yeah well ..
//   (there may be a way to find out if '\b' alone would work though)
static void
ttyBack(void)
{
    char key;
    key = '\b';
    write(1, &key, 1);
    key = ' ';
    write(1, &key, 1);
    key = '\b';
    write(1, &key, 1);
}

static char*
getConsoleInput(Hell_Console* console)
{
    static char text[MAX_EDIT_LINE];
    static bool newline = true;
    if (newline)
    {
        write(1, ">> ", 3);
        newline = false;
    }
    char c;
    int  avail = read(0, &c, 1);
    if (avail != -1)
    {
        if (c == console->eraseCode || c == 127 || c == 8)
        {
            if (console->console.cursor > 0)
            {
                console->console.buffer[--console->console.cursor] = '\0';
                ttyBack();
            }
            return NULL;
        }
        if (c == '\n')
        {
            strncpy(text, console->console.buffer, MAX_EDIT_LINE);
            // text[ttyConsole.cursor + 1] = '\0';
            fieldClear(&console->console);
            newline = true;
            write(1, &c, 1);
            return text;
        }
        console->console.buffer[console->console.cursor++] = c;
        write(1, &c, 1);
    }
    return NULL;
}

static void
pushEvent(Hell_EventQueue* queue, Hell_Event event)
{
    queue->queue[queue->head] = event;
    queue->head               = (queue->head + 1) % MAX_QUEUE_EVENTS;
}

void
hell_CreateEventQueue(Hell_EventQueue* queue)
{
    memset(queue, 0, sizeof(Hell_EventQueue));
}

void
hell_CreateConsole(Hell_Console* console)
{
#ifdef UNIX
    struct termios tc;
    if (isatty(STDIN_FILENO) != 1)
        hell_Error(0, "stdin is not a tty, tty console mode failed");
    if (tcgetattr(0, &tc) != 0)
        hell_Error(0, "tcgetattr failed");
    if (globalConsoleCounter == 0)
    {
        fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | FNDELAY);
        origTc = tc;
        tc.c_lflag &= ~(ECHO | ICANON);
        tc.c_iflag &= ~(ISTRIP | INPCK);
        tc.c_cc[VMIN]  = 1;
        tc.c_cc[VTIME] = 0;
        tcsetattr(0, TCSADRAIN, &tc);
        globalConsoleCounter++;
    }
    console->eraseCode = origTc.c_cc[VERASE];
    console->eofCode   = origTc.c_cc[VEOF];
#elif defined(WINDOWS)
#endif
}

void
hell_StartClock(void)
{
    initTime();
}

bool 
hell_ClockStarted(void)
{
    return globalClockStarted;
}

void hell_CoagulateInput(Hell_EventQueue* queue, Hell_Console* console, uint32_t windowCount, Hell_Window* windows[windowCount])
{
    assert(queue);
    if (console)
    {
        char*        ci = getConsoleInput(console);
        if (ci)
        {
            Hell_Event ev = {
                .type = HELL_EVENT_TYPE_CONSOLE,
                .mask = HELL_EVENT_MASK_CONSOLE_BIT
            };
            // this should put a trailing null at the end of the data, but not sure
            // if we need this...
            // ev.data.consoleData.ptrLen = strnlen(ci, MAX_EDIT_LINE - 1) + 1;
            ev.data.consoleData.ptrLen = strnlen(ci, MAX_EDIT_LINE);
            ev.data.consoleData.ptr    = hell_Malloc(ev.data.consoleData.ptrLen);
            // should copy a null at the end
            memcpy(ev.data.consoleData.ptr, ci, ev.data.consoleData.ptrLen);
            ev.time = hell_Time();
            pushEvent(queue, ev);
        }
    }
    for (int i = 0; i < windowCount; i++)
    {
        hell_DrainWindowEvents(queue, windows[i]);
    }
}

void
hell_SolveInput(Hell_EventQueue* queue)
{
    Hell_Event* event;
    for (; queue->tail != queue->head;
         queue->tail = (queue->tail + 1) % MAX_QUEUE_EVENTS)
    {
        event = &queue->queue[queue->tail];
        // hell_Announce("Event: type %d time %ld \n", event->type,
        // event->time);
        for (int i = 0; i < queue->subscriberCount; i++)
        {
            if (queue->subscriptions[i].eventMask & event->mask)
                if (queue->subscriptions[i].func(event, queue->subscriptions[i].data))
                    break; // if func returns true the event break from the loop
        }
        if (event->type == HELL_EVENT_MASK_CONSOLE_BIT)
        {
            hell_Free(event->data.consoleData.ptr);
        }
    }
}

void
hell_Subscribe(Hell_EventQueue* queue, Hell_EventMask mask, Hell_SubscriberFn func,
                           void* data)
{
    queue->subscriptions[queue->subscriberCount++] =
        (Hell_Subscription){
            .data = data, 
            .func = func, 
            .eventMask = mask};
}

uint64_t
hell_Time()
{
#ifdef UNIX
    return getUnixMicroSeconds();
#elif defined(WINDOWS)
    return getWinMicroSeconds();
#endif
}

void
hell_PushMouseDownEvent(Hell_EventQueue* queue, int16_t x, int16_t y,
                        uint8_t buttonCode)
{
    Hell_Event ev = {
        .type = HELL_EVENT_TYPE_MOUSEDOWN,
        .mask = HELL_EVENT_MASK_MOUSE_BIT,
        .time = hell_Time(),
    };
    ev.data.mouseData.x          = x;
    ev.data.mouseData.y          = y;
    ev.data.mouseData.buttonCode = buttonCode;
    pushEvent(queue, ev);
}

void
hell_PushMouseUpEvent(Hell_EventQueue* queue, int16_t x, int16_t y,
                      uint8_t buttonCode)
{
    Hell_Event ev = {
        .type = HELL_EVENT_TYPE_MOUSEUP,
        .mask = HELL_EVENT_MASK_MOUSE_BIT,
        .time = hell_Time(),
    };
    ev.data.mouseData.x          = x;
    ev.data.mouseData.y          = y;
    ev.data.mouseData.buttonCode = buttonCode;
    pushEvent(queue, ev);
}

void
hell_PushMouseMotionEvent(Hell_EventQueue* queue, int16_t x, int16_t y,
                          uint8_t buttonCode)
{
    Hell_Event ev = {
        .type = HELL_EVENT_TYPE_MOTION,
        .mask = HELL_EVENT_MASK_MOUSE_BIT,
        .time = hell_Time(),
    };
    ev.data.mouseData.x          = x;
    ev.data.mouseData.y          = y;
    ev.data.mouseData.buttonCode = buttonCode;
    pushEvent(queue, ev);
}

void
hell_PushKeyDownEvent(Hell_EventQueue* queue, uint32_t keyCode)
{
    Hell_Event ev = {
        .type = HELL_EVENT_TYPE_KEYDOWN, .mask = HELL_EVENT_MASK_KEY_BIT, .time = hell_Time()};
    ev.data.keyCode = keyCode;
    pushEvent(queue, ev);
}

void
hell_PushKeyUpEvent(Hell_EventQueue* queue, uint32_t keyCode)
{
    Hell_Event ev = {
        .type = HELL_EVENT_TYPE_KEYUP, .mask = HELL_EVENT_MASK_KEY_BIT, .time = hell_Time()};
    ev.data.keyCode = keyCode;
    pushEvent(queue, ev);
}

void
hell_PushWindowResizeEvent(Hell_EventQueue* queue, unsigned int width,
                           unsigned int height)
{
    Hell_Event ev = {
        .time = hell_Time(), .type = HELL_EVENT_TYPE_RESIZE, .mask = HELL_EVENT_MASK_WINDOW_BIT};
    ev.data.resizeData.width  = width;
    ev.data.resizeData.height = height;
    pushEvent(queue, ev);
}

void
hell_PushEmptyEvent(Hell_EventQueue* queue)
{
    Hell_Event ev = {.time = hell_Time()};
    pushEvent(queue, ev);
}

void
hell_DestroyEventQueue(Hell_EventQueue* queue)
{
    memset(queue, 0, sizeof(Hell_EventQueue));
}

void
hell_DestroyConsole(Hell_Console* console)
{
    memset(console, 0, sizeof(Hell_Console));
    globalConsoleCounter--;
    if (globalConsoleCounter == 0)
    {
#ifdef UNIX
        tcsetattr(0, TCSADRAIN, &origTc);
        fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) & ~FNDELAY);
#endif
        hell_Announce("Terminal control released.\n");
    }
    hell_Announce("Destroyed console.\n");
}

void
hell_Unsubscribe(Hell_EventQueue*          queue,
                               const Hell_SubscriberFn fn)
{
    assert(queue->subscriberCount > 0);
    hell_DPrint("Unsubscribing fn...\n");
    int fnIndex = -1;
    for (int i = 0; i < queue->subscriberCount; i++)
    {
        if (queue->subscriptions[i].func == fn)
        {
            fnIndex = i;
            break;
        }
    }
    if (fnIndex != -1)
        memmove(queue->subscriptions + fnIndex,
                queue->subscriptions + fnIndex + 1,
                (--queue->subscriberCount - fnIndex) *
                    sizeof(*queue->subscriptions)); // should only decrement the
                                                    // count if fnIndex is 0
}

uint64_t hell_SizeOfConsole(void)
{
    return sizeof(Hell_Console);
}

uint64_t hell_SizeOfEventQueue(void)
{
    return sizeof(Hell_EventQueue);
}

uint64_t hell_SizeOfWindow(void)
{
    return sizeof(Hell_Window);
}

Hell_Window* hell_AllocWindow(void)
{
    return hell_Malloc(sizeof(Hell_Window));
}

Hell_EventQueue* hell_AllocEventQueue(void)
{
    return hell_Malloc(sizeof(Hell_EventQueue));
}

Hell_Console* hell_AllocConsole(void)
{
    return hell_Malloc(sizeof(Hell_Console));
}
