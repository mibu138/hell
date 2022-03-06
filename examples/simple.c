#include "hell/common.h"

Hell_Hellmouth* hellmouth;

void frame(i64 fi, i64 dt)
{
    int evcount;
    const Hell_Event* events = hell_GetEvents(hellmouth, &evcount);
    hell_Print("Event count %d\n", evcount);
    if (fi > 400)
        hell_CloseAndExit(hellmouth);
}

int hellmain(void)
{
    Hell_Grimoire*   grimoire   = hell_Malloc(hell_SizeOfGrimoire());
    Hell_Console*    console    = hell_Malloc(hell_SizeOfConsole());
    Hell_EventQueue* eventQueue = hell_Malloc(hell_SizeOfEventQueue());
    Hell_Window*     window     = hell_Malloc(hell_SizeOfWindow());
    hellmouth  = hell_Malloc(hell_SizeOfHellmouth());
    hell_CreateEventQueue(eventQueue);
    hell_CreateGrimoire(eventQueue, grimoire);
    hell_CreateConsole(console);
    hell_CreateWindow(eventQueue, 500, 500, NULL, window);
    hell_CreateHellmouth(grimoire, eventQueue, console, 1, &window, frame, NULL, hellmouth);
    hell_Loop(hellmouth);
    return 0;
}

#ifdef WIN32
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
    hell_SetHinstance(hInstance);
    hellmain();
    return 0;
}
#elif UNIX
int main(int argc, char* argv[])
{
    hellmain();
}
#endif
