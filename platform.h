#ifndef HELL_PLATFORM_H
#define HELL_PLATFORM_H

#ifdef _WIN32
#define WINDOWS
#include <windows.h>
#endif
#ifdef __linux__
#define UNIX
#endif

#endif // end of HELL_PLATHFORM_H include gaurd
