#include "hell/common.h"

int main(int argc, char *argv[])
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
