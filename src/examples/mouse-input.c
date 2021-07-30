#include "hell/hell.h"

bool mouseEventHandler1(const Hell_Event* event, void* data)
{
    hell_Print("fn1 :: x: %d y: %d\n", hell_GetMouseX(event), hell_GetMouseY(event));
    return true;
}

bool mouseEventHandler2(const Hell_Event* event, void* data)
{
    hell_Print("fn2 :: x: %d y: %d\n", hell_GetMouseX(event), hell_GetMouseY(event));
    return true;
}

Hell_Window* window1;
Hell_Window* window2;

static void userFrame(void)
{
    hell_Print("window1 :: w: %d h: %d\n", hell_GetWindowWidth(window1), hell_GetWindowHeight(window1));
    hell_Print("window2 :: w: %d h: %d\n", hell_GetWindowWidth(window2), hell_GetWindowHeight(window2));
}

int main(int argc, char *argv[])
{
    window1   = hell_AllocWindow();
    window2   = hell_AllocWindow();
    Hell_Console*    console   = hell_AllocConsole();
    Hell_EventQueue* queue     = hell_AllocEventQueue();
    Hell_Grimoire*   grimoire  = hell_AllocGrimoire();
    Hell_Hellmouth*  hellmouth = hell_AllocHellmouth();
    hell_CreateConsole(console);
    hell_CreateEventQueue(queue);
    hell_CreateGrimoire(queue, grimoire);
    hell_CreateWindow(queue, 666, 666, 0, window1);
    hell_CreateWindow(queue, 400, 700, 0, window2);
    Hell_Window* windows[2] = {window1, window2};
    hell_CreateHellmouth(grimoire, queue, console, 2, windows, userFrame, NULL, hellmouth);

    hell_Subscribe(queue, HELL_EVENT_MASK_MOUSE_BIT | HELL_EVENT_MASK_WINDOW_BIT, 1, mouseEventHandler1, NULL);
    hell_Subscribe(queue, HELL_EVENT_MASK_MOUSE_BIT | HELL_EVENT_MASK_WINDOW_BIT, 2, mouseEventHandler2, NULL);

    hell_Loop(hellmouth);
    return 0;
}
