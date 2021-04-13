#ifndef HELL_COMMON_H
#define HELL_COMMON_H

#include <stdbool.h>
#include "types.h"

typedef enum {
	HELL_ERR_FATAL, 
} Hell_ErrorCode;

void     hell_Print(const char* fmt, ...);
void     hell_Print_Vec3(const float[3]);
void     hell_Print_Mat4(const float[4][4]);
void     hell_DPrint(const char* fmt, ...);
void     hell_Announce(const char* fmt, ...);
void     hell_Error( Hell_ErrorCode code, const char *fmt, ... );

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
void     hell_Init(void);

// run run run and never return
void     hell_Loop(void);

// uber function that call the individual shut down functions in the correct order
void     hell_ShutDown(void);

// calls shutdown and exits the program
void     hell_Quit(void);


#endif /* end of include guard: HELL_COM_H */
