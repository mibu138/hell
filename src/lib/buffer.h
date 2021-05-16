#ifndef HELL_DATA_STRUCTURES_H
#define HELL_DATA_STRUCTURES_H

// Data Structures

#define HELL_SIMPLE_TYPE_NAMES
#include "types.h"

typedef struct Hell_Buffer {
    Byte*  data;
    Size   maxSize;
    Size   curSize;
} Hell_Buffer;

void hell_BufferInit(Hell_Buffer* buf, Byte* data, Size length);

#endif /* end of include guard: HELL_DATA_H */
