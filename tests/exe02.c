#include <hell/hell.h>
#include <math.h>
#include <stdlib.h>

Hell_Hellmouth hm;

void frame(u64 fn, u64 dt)
{
    Hell_Tick t = hell_Time();
    hell_Print("T: %d\n", t);
    if (t > 100000)
    {
        hell_Print("Ending\n");
        hell_CloseHellmouth(&hm);
        hell_Exit(0);
    }
}

int main(int argc, char *argv[])
{
    hell_OpenHellmouth(frame, NULL, &hm);
    for (int i = 0; i < 10; i++)
    {
        u16 w = rand() % 1000;
        u16 h = rand() % 1000;
        hell_Print("i %d w %d h %d \n", i, w, h);
        hell_HellmouthAddWindow(&hm, w, h, NULL);
    }
    hell_Loop(&hm);
}
