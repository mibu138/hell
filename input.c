#include "window.h"
#include "input.h"
#include "common.h"
#include "cmd.h"
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "private.h"
#include "platform.h"
#include "debug.h"
//
#ifdef UNIX
#include <termios.h>
static struct termios origTc; // stash the tc at program start here so we can reset it on shut down
#elif defined(WINDOWS)
#include <windows.h>
#endif

static_assert(sizeof(Hell_I_EventType) == 4, "sizeof(Hell_I_EventType) should be 4");
static_assert(sizeof(Hell_I_EventMask) == 4, "sizeof(Hell_I_EventMask) should be 4");
static_assert(sizeof(Hell_I_Event) == 32,    "sizeof(Hell_I_Event) should be 32, to allow 2 events to be read with each cacheline read (assuming the event queue is aligned well)");

typedef struct {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
} Field;
//
//
// returning true consumes the event

#define MAX_SUBSCRIBERS 32

typedef struct {
    Hell_I_SubscriberFn func;
    Hell_I_EventMask    eventMask;
} Hell_I_Subscription;

static Hell_I_Subscription subscriptions[MAX_SUBSCRIBERS];
static int subscriberCount;

static bool   consoleActive;

//
// bk000306: upped this from 64
#define	MAX_QUEUE_EVENTS    64
#define	MASK_QUEUE_EVENTS	( MAX_QUED_EVENTS - 1 )

Hell_I_Event eventQueue[MAX_QUEUE_EVENTS];
// bk000306: initialize
int   eventHead = 0;
int   eventTail = 0;


static int ttyEraseCode;
static int ttyEofCode;

static Field ttyConsole;

static_assert (STDIN_FILENO == 0, "We assume the the fd for stdin is 0");

#ifdef UNIX
static struct timespec unixEpoch;
#define UNIX_CLOCK_ID CLOCK_MONOTONIC

static void initUnixTime(void)
{
    clock_gettime(UNIX_CLOCK_ID, &unixEpoch);
}

static uint64_t getUnixMicroSeconds(void)
{
    struct timespec curTime;
    clock_gettime(UNIX_CLOCK_ID, &curTime);
    time_t s = curTime.tv_sec - unixEpoch.tv_sec;
    long   ns = curTime.tv_nsec - unixEpoch.tv_nsec;
    uint64_t ms = s * 1000000 + ns / 1000;
    return ms;
}

#elif defined(WINDOWS)

static LARGE_INTEGER winEpoch;
static LARGE_INTEGER winFreq;

static void initWinTime(void)
{
    QueryPerformanceFrequency(&winFreq);
    QueryPerformanceCounter(&winEpoch);
}

static uint64_t getWinMicroSeconds(void)
{
    LARGE_INTEGER curTicks, elapsedTime;
    QueryPerformanceCounter(&curTicks);
    elapsedTime.QuadPart  = curTicks.QuadPart - winEpoch.QuadPart;
    elapsedTime.QuadPart *= 1000000;
    assert(winFreq.QuadPart > 0);
    elapsedTime.QuadPart /= winFreq.QuadPart;
    return elapsedTime.QuadPart;
}

#else 
#error
#endif

static void initTime(void)
{
#ifdef UNIX
    initUnixTime();
#elif defined(WINDOWS)
    initWinTime();
#else
#error
#endif
}

static void fieldClear(Field* field)
{
    memset(field, 0, sizeof(Field));
}
// do a backspace
// TTimo NOTE: it seems on some terminals just sending '\b' is not enough
//   so for now, in any case we send "\b \b" .. yeah well ..
//   (there may be a way to find out if '\b' alone would work though)
static void ttyBack(void)
{
  char key;
  key = '\b';
  write(1, &key, 1);
  key = ' ';
  write(1, &key, 1);
  key = '\b';
  write(1, &key, 1);
}

static void initConsoleInput(void)
{
#ifdef UNIX
    fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
    struct termios tc;
    if(isatty(STDIN_FILENO)!=1)
        hell_Error(0, "stdin is not a tty, tty console mode failed");
    if (tcgetattr(0, &tc) != 0)
        hell_Error(0, "tcgetattr failed");
    origTc = tc;
    ttyEraseCode = tc.c_cc[VERASE];
    ttyEofCode   = tc.c_cc[VEOF];
    tc.c_lflag &= ~(ECHO | ICANON);
    tc.c_iflag &= ~(ISTRIP | INPCK);
    tc.c_cc[VMIN] = 1;
    tc.c_cc[VTIME] = 0;
    tcsetattr(0, TCSADRAIN, &tc);
#elif defined(WINDOWS)
#endif
    hell_Announce("Started console.\n");
}

static char* getConsoleInput(void)
{
    if (!consoleActive) return NULL;
    static char text[MAX_EDIT_LINE];
    static bool newline = true;
    if (newline)
    {
        write(1, ">> ", 3);
        newline = false;
    }
    char c;
    int avail = read(0, &c, 1);
    if (avail != -1)
    {
        if (c == ttyEraseCode || c == 127 || c == 8)
        {
            if (ttyConsole.cursor > 0)
            {
                ttyConsole.buffer[--ttyConsole.cursor] = '\0';
                ttyBack();
            }
            return NULL;
        }
        if (c == '\n')
        {
            strncpy(text, ttyConsole.buffer, MAX_EDIT_LINE);
            //text[ttyConsole.cursor + 1] = '\0';
            fieldClear(&ttyConsole);
            newline = true;
            write(1, &c, 1);
            return text;
        }
        ttyConsole.buffer[ttyConsole.cursor++] = c;
        write(1, &c, 1);
    }
    return NULL;
}

static void pushEvent(Hell_I_Event event)
{
    eventQueue[eventHead] = event;
    eventHead = (eventHead + 1) % MAX_QUEUE_EVENTS;
}

void hell_i_Init(bool initConsole)
{
    initTime();
    if (initConsole)
    {
        initConsoleInput();
        consoleActive = true;
    }
}

void hell_i_PumpEvents(void)
{
    char* ci = getConsoleInput();
    if (ci)
    {
        Hell_I_Event ev = {
            .type = HELL_I_CONSOLE,
        };
        // this should put a trailing null at the end of the data, but not sure if we need this...
        //ev.data.consoleData.ptrLen = strnlen(ci, MAX_EDIT_LINE - 1) + 1; 
        ev.data.consoleData.ptrLen = strnlen(ci, MAX_EDIT_LINE); 
        ev.data.consoleData.ptr = hell_Malloc(ev.data.consoleData.ptrLen);
        memcpy(ev.data.consoleData.ptr, ci, ev.data.consoleData.ptrLen); // should copy a null at the end
        ev.time = hell_Time();
        pushEvent(ev);
    }
    hell_w_DrainWindowEvents();
}

void hell_i_DrainEvents(void)
{
    Hell_I_Event* event;
    for ( ; eventTail != eventHead; eventTail = (eventTail + 1) % MAX_QUEUE_EVENTS) 
    {
        event = &eventQueue[eventTail];   
        //hell_Announce("Event: type %d time %ld \n", event->type, event->time);
        if (event->type == HELL_I_CONSOLE)
        {
            hell_c_AddNText(event->data.consoleData.ptr, event->data.consoleData.ptrLen);
            hell_c_AddChar('\n');
            hell_Free(event->data.consoleData.ptr);
            continue;
        }
        for (int i = 0; i < subscriberCount; i++) 
        {
            if (subscriptions[i].eventMask & event->mask)
                if ( subscriptions[i].func(event) ) break; // if func returns true the event break from the loop
        }
    }
}

void hell_i_Subscribe(Hell_I_SubscriberFn func, Hell_I_EventMask mask)
{
    subscriptions[subscriberCount++] = (Hell_I_Subscription){func, mask};
}

uint64_t hell_Time()
{
#ifdef UNIX
    return getUnixMicroSeconds();
#elif defined(WINDOWS)
    return getWinMicroSeconds();
#endif
}

void hell_i_PushMouseDownEvent(int16_t x, int16_t y, uint8_t buttonCode)
{
    Hell_I_Event ev = {
        .type = HELL_I_MOUSEDOWN,
        .mask = HELL_I_MOUSE_BIT,
        .time = hell_Time(),
    };
    ev.data.mouseData.x = x;
    ev.data.mouseData.y = y;
    ev.data.mouseData.buttonCode = buttonCode;
    pushEvent(ev);
}

void hell_i_PushMouseUpEvent(int16_t x, int16_t y, uint8_t buttonCode)
{
    Hell_I_Event ev = {
        .type = HELL_I_MOUSEUP,
        .mask = HELL_I_MOUSE_BIT,
        .time = hell_Time(),
    };
    ev.data.mouseData.x = x;
    ev.data.mouseData.y = y;
    ev.data.mouseData.buttonCode = buttonCode;
    pushEvent(ev);
}

void hell_i_PushMouseMotionEvent(int16_t x, int16_t y, uint8_t buttonCode)
{
    Hell_I_Event ev = {
        .type = HELL_I_MOTION,
        .mask = HELL_I_MOUSE_BIT,
        .time = hell_Time(),
    };
    ev.data.mouseData.x = x;
    ev.data.mouseData.y = y;
    ev.data.mouseData.buttonCode = buttonCode;
    pushEvent(ev);
}

void hell_i_PushKeyDownEvent(uint32_t keyCode)
{
    Hell_I_Event ev = {
        .type = HELL_I_KEYDOWN,
        .mask = HELL_I_KEY_BIT,
        .time = hell_Time()
    };
    ev.data.keyCode = keyCode;
    pushEvent(ev);
}

void hell_i_PushKeyUpEvent(uint32_t keyCode)
{
    Hell_I_Event ev = {
        .type = HELL_I_KEYUP,
        .mask = HELL_I_KEY_BIT,
        .time = hell_Time()
    };
    ev.data.keyCode = keyCode;
    pushEvent(ev);
}

void hell_i_PushWindowResizeEvent(unsigned int width, unsigned int height)
{
    Hell_I_Event ev = {
        .time = hell_Time(),
        .type = HELL_I_RESIZE,
        .mask = HELL_I_WINDOW_BIT
    };
    ev.data.resizeData.width = width;
    ev.data.resizeData.height = height;
    pushEvent(ev);
}

void hell_i_PushEmptyEvent(void)
{
    Hell_I_Event ev = {
        .time = hell_Time()
    };
    pushEvent(ev);
}

void hell_i_CleanUp(void)
{
    memset(subscriptions, 0, sizeof(subscriptions));
    subscriberCount = 0;
    memset(eventQueue, 0, sizeof(eventQueue));
    eventHead = 0;
    eventTail = 0;
    ttyEofCode = 0;
    ttyEofCode = 0;
    memset(&ttyConsole, 0, sizeof(ttyConsole));
    if (consoleActive)
    {
        #ifdef UNIX
        tcsetattr(0, TCSADRAIN, &origTc);
        fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
        #endif
    }
    hell_Announce("Input shutdown.\n");
}

void hell_i_Unsubscribe(const Hell_I_SubscriberFn fn)
{
    assert(subscriberCount > 0);
    hell_DPrint("Unsubscribing fn...\n");
    int fnIndex = -1;
    for (int i = 0; i < subscriberCount; i++)
    {
        if (subscriptions[i].func == fn)
        {
            fnIndex = i;
            break;
        }
    }
    if (fnIndex != -1)
        memmove(subscriptions + fnIndex, 
                subscriptions + fnIndex + 1, 
                (--subscriberCount - fnIndex) * sizeof(*subscriptions)); // should only decrement the count if fnIndex is 0
}
