#ifndef HELL_COMMON_H
#define HELL_COMMON_H

typedef enum {
	HELL_ERR_FATAL, 
} Hell_ErrorCode;

void hell_Print(const char* fmt, ...);
void hell_Error( Hell_ErrorCode code, const char *fmt, ... );

#endif /* end of include guard: HELL_COM_H */
