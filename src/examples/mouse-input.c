#include "hell.h"

bool mouseEventHandler(const Hell_Event* event, void* data)
{
    hell_Print("x: %d y: %d\n", event->data.mouseData.x, event->data.mouseData.y);
    return true;
}

int main(int argc, char *argv[])
{
    Hell_Window*     window1   = hell_AllocWindow();
    Hell_Window*     window2   = hell_AllocWindow();
    Hell_Console*    console   = hell_AllocConsole();
    Hell_EventQueue* queue     = hell_AllocEventQueue();
    Hell_Grimoire*   grimoire  = hell_AllocGrimoire();
    Hell_Hellmouth*  hellmouth = hell_AllocHellmouth();
    hell_CreateConsole(console);
    hell_CreateEventQueue(queue);
    hell_CreateGrimoire(queue, grimoire);
    hell_CreateWindow(666, 666, 0, window1);
    hell_CreateWindow(400, 700, 0, window2);
    Hell_Window* windows[2] = {window1, window2};
    hell_CreateHellmouth(grimoire, queue, console, 2, windows, NULL, NULL, hellmouth);

    hell_Subscribe(queue, HELL_EVENT_MASK_MOUSE_BIT, mouseEventHandler, NULL);

    hell_Loop(hellmouth);
    return 0;
}
