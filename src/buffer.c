#include "buffer.h"
#include <memory.h>

void hell_BufferInit(Hell_Buffer* buf, Byte* data, Size length)
{
    memset(buf, 0, sizeof(*buf));
    buf->data = data;
    buf->maxSize = length;
}
