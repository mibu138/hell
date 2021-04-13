#ifndef HELL_MEM_H
#define HELL_MEM_H

#include <stdlib.h>

void* hell_m_Alloc(size_t size);
void  hell_m_Free(void* ptr);
char* hell_m_CopyString(const char* in);

#endif /* end of include guard: MEMORY_H */
