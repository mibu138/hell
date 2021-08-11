#include "hell/common.h"

int hellmain(void)
{
    Hell_Grimoire*   grimoire   = hell_Malloc(hell_SizeOfGrimoire());
    Hell_Console*    console    = hell_Malloc(hell_SizeOfConsole());
    Hell_EventQueue* eventQueue = hell_Malloc(hell_SizeOfEventQueue());
    Hell_Window*     window     = hell_Malloc(hell_SizeOfWindow());
    Hell_Hellmouth*  hellmouth  = hell_Malloc(hell_SizeOfHellmouth());
    hell_CreateEventQueue(eventQueue);
    hell_CreateGrimoire(eventQueue, grimoire);
    hell_CreateConsole(console);
    hell_CreateWindow(eventQueue, 500, 500, NULL, window);
    hell_CreateHellmouth(grimoire, eventQueue, console, 1, &window, NULL, NULL, hellmouth);
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
