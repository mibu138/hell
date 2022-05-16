// basic types. no include guard so that we can simplify the names at will.
// should only be simple typedefs otherwise we well get redefinition errors.

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  Hell_Byte;
typedef uint64_t Hell_Size;
typedef int64_t Hell_Tick;
typedef int64_t Hell_Frame;
typedef uint16_t Hell_WindowID;
#define HELL_WINDOW_ID_MAX UINT16_MAX

#ifdef HELL_SIMPLE_TYPE_NAMES
typedef Hell_Byte Byte;
typedef Hell_Size Size;
typedef Hell_Tick Tick;
typedef Hell_Frame Frame;
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
