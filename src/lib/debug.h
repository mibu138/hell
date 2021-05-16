#ifndef HELL_DEBUG_H
#define HELL_DEBUG_H

#ifdef NDEBUG
#define HELL_DEBUG_PRINTING 0
#else
#define HELL_DEBUG_PRINTING 1
#endif

#define HELL_DEBUG_TAG_NO_TAG   "NO_TAG"

// do not call directly. use hell_DPrint.
// reason is so we can enable logging in the debug m
void     hell_DebugPrintImpl(const char* fmt, ...);

#define hell_DebugPrint(tag, fmt, ...) \
    do {if (HELL_DEBUG_PRINTING) \
        { \
            hell_DebugPrintImpl(tag); \
            hell_DebugPrintImpl(":%s:%d:%s(): ", __FILE__, __LINE__, __func__); \
            hell_DebugPrintImpl(fmt, ##__VA_ARGS__); \
        } \
    } while (0)

#define hell_DPrint(fmt, ...) \
    do {if (HELL_DEBUG_PRINTING) \
        { \
            hell_DebugPrintImpl(HELL_DEBUG_TAG_NO_TAG); \
            hell_DebugPrintImpl(":%s:%d:%s(): ", __FILE__, __LINE__, __func__); \
            hell_DebugPrintImpl(fmt, ##__VA_ARGS__); \
        } \
    } while (0)


// these only accept string literals.... 
void hell_AddFilterTag(const char* tag);
void hell_AddFilterTags(const unsigned int count, const char* tags[count]);

#endif /* end of include guard: HELL_DEBUG_H */

