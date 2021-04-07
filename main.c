#include <stdio.h>
#include <unistd.h>
#include "cmd.h"
#include "input.h"
#include "common.h"
#include "display.h"
#include "platform.h"
#ifdef WINDOWS
#include "win_local.h"
#endif

static void chungus(void)
{
    hell_Print("Aw lawd %s comin!\n", hell_c_Argv(1));
}

int hell_Main(void)
{
    hell_Announce("Starting.\n");
    hell_i_Init(false);
    hell_c_Init();
    hell_d_Init(500, 500, 0);
    hell_c_AddCommand("chungus", chungus);
    while (1)
    {
        hell_i_PumpEvents();
        hell_i_DrainEvents();
        hell_c_Execute();
        usleep(16000);
    }
    hell_Announce("End.\n");
    return 0;
}

#if defined(UNIX)
int main(int argc, char *argv[])
{
    return hell_Main();
}
#elif defined(WINDOWS)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    winVars.instance = hInstance;
    return hell_Main();
}

#endif

