#ifndef HELL_WIN_LOCAL_H
#define HELL_WIN_LOCAL_H

#include <windows.h>

typedef struct {
    HINSTANCE instance;
} HellWinVars;

extern HellWinVars winVars;

#endif