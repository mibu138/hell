#define HELL_INIT(framefn) \
    eventQueue = hell_AllocEventQueue();\
    grimoire   = hell_AllocGrimoire();\
    console    = hell_AllocConsole();\
    window     = hell_AllocWindow();\
    hellmouth  = hell_AllocHellmouth();\
    hell_CreateConsole(console);\
    hell_CreateEventQueue(eventQueue);\
    hell_CreateGrimoire(eventQueue, grimoire);\
    hell_CreateWindow(eventQueue, 666, 666, NULL, window);\
    hell_CreateHellmouth(grimoire, eventQueue, console, 1, &window, framefn, NULL, hellmouth)
