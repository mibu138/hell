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

#endif /* end of include guard: HELL_COM_H */
