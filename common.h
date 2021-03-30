#ifndef HELL_COMMON_H
#define HELL_COMMON_H

#include <stdint.h>

typedef enum {
	HELL_ERR_FATAL, 
} Hell_ErrorCode;

void     hell_Print(const char* fmt, ...);
void     hell_Announce(const char* fmt, ...);
void     hell_Error( Hell_ErrorCode code, const char *fmt, ... );
// returns time since epoch in microseconds
uint64_t hell_Time(void);
uint64_t hell_Align(const uint64_t quantity, const uint32_t alignment);
void     hell_BitPrint(const void* const thing,  const uint32_t bitcount);
void     hell_BytePrint(const void* const thing, const uint32_t byteCount);

#endif /* end of include guard: HELL_COM_H */
