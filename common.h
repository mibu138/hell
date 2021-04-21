#ifndef HELL_COMMON_H
#define HELL_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include "types.h"

typedef enum {
	HELL_ERR_FATAL, 
    HELL_ERR_MILD
} Hell_ErrorCode;

struct Hell_Window;

typedef void (*Hell_FrameFn)(void);
typedef void (*Hell_ShutDownFn)(void);

void     hell_Print(const char* fmt, ...);
void     hell_Print_Vec3(const float[3]);
void     hell_Print_Mat4(const float[4][4]);
void     hell_Announce(const char* fmt, ...);
void     hell_Error( Hell_ErrorCode code, const char *fmt, ... );

void*    hell_Malloc(size_t size);
void     hell_Free(void* ptr);
char*    hell_CopyString(const char* in);

// returns time since epoch in microseconds
Hell_Tick hell_Time(void);
uint64_t  hell_Align(const uint64_t quantity, const uint32_t alignment);
void      hell_BitPrint(const void* const thing,  const uint32_t bitcount);
void      hell_BytePrint(const void* const thing, const uint32_t byteCount);

// sleep for s seconds
void     hell_Sleep(double s);

// platform agnostic library loading
void*    hell_LoadLibrary(const char* name);
void*    hell_LoadSymbol(void* module, const char* symname);

bool     hell_FileExists(const char* path);

// uber function that calls the individual initializers in the correct order
void     hell_Init(bool initConsole, Hell_FrameFn userFrame, Hell_ShutDownFn userShutDown, const struct Hell_Window** window);

// run run run and never return
void     hell_Loop(void);

void     hell_Frame(Hell_Tick dt); // run single frame

// uber function that call the individual shut down functions in the correct order
void     hell_ShutDown(void);

// calls shutdown and exits the program
void     hell_Quit(void);


#endif /* end of include guard: HELL_COM_H */
