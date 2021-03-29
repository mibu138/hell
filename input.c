#include "common.h"
#include "cmd.h"
#include "mem.h"
#include <fcntl.h>
#include <termios.h>
#include <assert.h>
#include <unistd.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
#include "private.h"
//

typedef struct {
	int		cursor;
	int		scroll;
	int		widthInChars;
	char	buffer[MAX_EDIT_LINE];
} Field;
//
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
//
// returning true consumes the event
typedef bool (*Hell_I_SubscriberFn)(const Hell_I_Event*);

#define MAX_SUBSCRIBERS 32

static Hell_I_SubscriberFn subscribers[MAX_SUBSCRIBERS];
static int subscriberCount;

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
    fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
    struct termios tc;
    if(isatty(STDIN_FILENO)!=1)
        hell_Error(0, "stdin is not a tty, tty console mode failed");
    hell_Print("Started console.\n");
    if (tcgetattr(0, &tc) != 0)
        hell_Error(0, "tcgetattr failed");
    ttyEraseCode = tc.c_cc[VERASE];
    ttyEofCode   = tc.c_cc[VEOF];
    tc.c_lflag &= ~(ECHO | ICANON);
    tc.c_iflag &= ~(ISTRIP | INPCK);
    tc.c_cc[VMIN] = 1;
    tc.c_cc[VTIME] = 0;
    tcsetattr(0, TCSADRAIN, &tc);
}

static char* getConsoleInput(void)
{
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

void hell_i_Init(void)
{
    initConsoleInput();
}

void hell_i_SourceEvents(void)
{
    char* ci = getConsoleInput();
    if (ci)
    {
        Hell_I_Event ev = {
            .type = HELL_I_CONSOLE,
        };
        ev.ptrLen = strnlen(ci, MAX_EDIT_LINE - 1) + 1;
        ev.ptr = hell_m_Alloc(ev.ptrLen);
        memcpy(ev.ptr, ci, ev.ptrLen); // should copy a null at the end
        pushEvent(ev);
    }
}

void hell_i_DrainEvents(void)
{
    Hell_I_Event* event;
    for ( ; eventTail != eventHead; eventTail = (eventTail + 1) % MAX_QUEUE_EVENTS) 
    {
        //printf("e");
        event = &eventQueue[eventTail];   
        if (event->type == HELL_I_CONSOLE)
        {
            hell_c_AddNText(event->ptr, event->ptrLen);
            hell_c_AddChar('\n');
        }
        else 
        {
            for (int i = 0; i < subscriberCount; i++) 
            {
                if ( subscribers[i](event) ) break;
            }
        }
    }
}
