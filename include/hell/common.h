#ifndef HELL_COMMON_H
#define HELL_COMMON_H

#include "cmd.h"
#include "input.h"
#include "types.h"
#include "window.h"
#include <stdbool.h>
#include <stddef.h>

typedef enum { HELL_ERR_FATAL, HELL_ERR_MILD } Hell_ErrorCode;

typedef void (*Hell_FrameFn)(Hell_Frame frameNumber,
                             Hell_Tick  dt /*microseconds*/);
typedef void (*Hell_ShutDownFn)(void);

typedef enum {
    HELL_OPTION_RECORD_INPUT_BIT = 1 << 0,
    HELL_OPTION_ENABLE_TTY_CONSOLE_BIT  = 1 << 1
} HellOptionFlagBits;

typedef struct Hell_Window Hell_Window;
typedef struct Hell_Mouth {
    Hell_Grimoire*   grimoire;
    Hell_EventQueue* eventqueue;
    Hell_Console*    console;
    Hell_Window**    windows;
    uint32_t         windowCount;
    Hell_FrameFn     userFrame;
    Hell_ShutDownFn  userShutDown;
    u64              frameCount;
    i64              targetFrameDuration;
    // points to a buffer on the stack in hell_Loop()
    Hell_Event*      frameEventStack;
    int              frameEventCount;
    uint32_t         options;
    HellArray        recorded_input;
} Hell_Mouth;

typedef Hell_Mouth HellContext;

void hell_Print(const char* fmt, ...);
// this one will insert a new line automatically
void hell_print(const char* fmt, ...);
void hell_Print_Vec3(const float[3]);
void hell_Print_Mat4(const float[4][4]);
void hell_Announce(const char* fmt, ...);
void hell_Error(Hell_ErrorCode code, const char* fmt, ...);
void hell_ErrorFatalImpl(const char* fmt, ...);

// the ## makes the variable args optional (otherwise you need to give at least
// 1)
#define hell_error_fatal(fmt, ...)                                             \
    hell_ErrorFatalImpl("%s:%d:%s() ...\n%s\n", __FILE__, __LINE__, __func__,  \
                        fmt, ##__VA_ARGS__)

// will error if allocation fails
void* hell_Malloc(size_t size);
// will error if allocation fails
void* hell_Realloc(void* ptr, size_t size);
void  hell_Free(void* ptr);
char* hell_CopyString(const char* in);

// returns time since epoch in microseconds
Hell_Tick hell_Time(void);

// return a smallest integer greater than or equal to quantity that satisfies
// alignment
uint64_t hell_Align(const uint64_t quantity, const uint32_t alignment);
void     hell_BitPrint(const void* const thing, const uint32_t bitcount);
void     hell_BytePrint(const void* const thing, const uint32_t byteCount);
void     hell_CreateHellmouth(Hell_Grimoire* grimoire, Hell_EventQueue* queue,
                              Hell_Console* console, uint32_t windowCount,
                              Hell_Window* windows[], Hell_FrameFn userFrame,
                              Hell_ShutDownFn userShutDown, Hell_Mouth* hellmouth);
// abreviated hellmouth creation that allocs and creates everythin
// returns 0 as success
int      hell_OpenMouth(Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown, uint32_t option_flags,
                        Hell_Mouth* hm);
void     hell_CloseHellmouth(Hell_Mouth* hellmouth);
void     hell_CloseAndExit(Hell_Mouth*);

Hell_Window* hell_HellmouthAddWindow(Hell_Mouth* hm, u16 w, u16 h,
                                     const char* name);
void         hell_Exit(int code);
// sleep for s seconds
void         hell_Sleep(double s);
void         hell_MicroSleep(uint64_t us);

// platform agnostic library loading
void* hell_LoadLibrary(const char* name);
void* hell_LoadSymbol(void* module, const char* symname);

bool hell_FileExists(const char* path);

// uber function that calls the individual initializers in the correct order

// run run run and never return
void hell_Loop(Hell_Mouth*);

void hell_Frame(Hell_Mouth*, Hell_Tick dt); // run single frame

const Hell_Event* hell_GetEvents(Hell_Mouth* h, int* event_count);

// uber function that call the individual shut down functions in the correct
// order
void hell_ShutDown(void);

// calls shutdown and exits the program
void hell_Quit(Hell_Grimoire* grim, void* hellmouthvoid);

uint64_t hell_SizeOfGrimoire(void);
uint64_t hell_SizeOfConsole(void);
uint64_t hell_SizeOfEventQueue(void);
uint64_t hell_SizeOfWindow(void);
uint64_t hell_SizeOfHellmouth(void);

Hell_Window*     hell_AllocWindow(void);
Hell_Console*    hell_AllocConsole(void);
Hell_EventQueue* hell_AllocEventQueue(void);
Hell_Grimoire*   hell_AllocGrimoire(void);
Hell_Mouth*      hell_AllocHellmouth(void);

static inline bool
hell_is_power_of_two(int64_t x)
{
    return x && (!(x & (x - 1)));
}


#ifdef HELL_SIMPLE_NAMES
#define HELL_SIMPLE_FUNCTION_NAMES
#define HELL_SIMPLE_TYPE_NAMES
#endif

#ifdef HELL_SIMPLE_FUNC_NAMES
#define HELL_SIMPLE_FUNCTION_NAMES
#endif
#ifdef HELL_SIMPLE_FUNCTION_NAMES
#define Print(...) hell_Print(__VA_ARGS__)
#define OpenMouth(...) hell_OpenMouth(__VA_ARGS__)
#define OpenHellmouth_NoConsole(...) hell_OpenHellmouth_NoConsole(__VA_ARGS__)
#define Loop(...) hell_Loop(__VA_ARGS__)
#define error_fatal hell_error_fatal
#define print hell_print
#define malloc hell_Malloc
#define free hell_Free
#endif

#ifdef hell_array_alloc2
#error "hell_array_alloc already defined!"
#else
#define hell_array_alloc(arr, count) ((arr) = hell_Malloc(sizeof(arr[0]) * (count)))
#endif

#ifdef HELL_SIMPLE_TYPE_NAMES
typedef Hell_Mouth Hellmouth;
typedef Hell_Mouth Mouth;
typedef Hell_Event Event;
#endif

#endif /* end of include guard: HELL_COM_H */
