#include <hell/hell.h>

Hell_Hellmouth* hm;

void frame(u64 fn, u64 dt)
{
    Hell_Tick t = hell_Time();
    hell_Print("T: %d\n", t);
    if (t > 500000)
    {
        hell_Print("Ending");
        hell_CloseHellmouth(hm);
        hell_Exit(0);
    }
}

int main(int argc, char *argv[])
{
    hm = hell_OpenHellmouth(frame, NULL);
    hell_HellmouthAddWindow(hm, 1000, 1000, NULL);
    hell_Loop(hm);
}
