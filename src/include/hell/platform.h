#ifndef HELL_PLATFORM_H
#define HELL_PLATFORM_H

#ifdef WIN32
#include <windows.h>
#elif defined(__linux__)
#define UNIX
#else
#error "Platform not supported"
#endif

#endif // end of HELL_PLATHFORM_H include gaurd
