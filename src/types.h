// basic types. no include guard so that we can simplify the names at will.
// should only be simple typedefs otherwise we well get redefinition errors.

#include <stdint.h>

typedef uint8_t  Hell_Byte;
typedef uint64_t Hell_Size;
typedef uint64_t Hell_Tick;

#ifdef HELL_SIMPLE_TYPE_NAMES
typedef Hell_Byte Byte;
typedef Hell_Size Size;
typedef Hell_Tick Tick;
#endif


