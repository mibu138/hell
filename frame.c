#include "common.h"
#include "cmd.h"
#include "window.h"
#include "input.h"
#include "client.h"
#include "server.h"
#include "stdlib.h"
#include <unistd.h>
#define HELL_SIMPLE_TYPE_NAMES
#include "types.h"

typedef Hell_C_Var Var;

const Var* vFps;

void hell_Init(void)
{
    hell_c_Init();
    hell_c_AddCommand("quit", hell_Quit);
    hell_i_Init(true);
    const Hell_C_Var* varW = hell_c_GetVar("d_width", "600", HELL_C_VAR_ARCHIVE_BIT);
    const Hell_C_Var* varH = hell_c_GetVar("d_height", "500", HELL_C_VAR_ARCHIVE_BIT);
    const Hell_C_Var* dedicated = hell_c_GetVar("dedicated", "0", 0);
    vFps = hell_c_GetVar("fps", "60", HELL_C_VAR_ARCHIVE_BIT);
    hell_w_Init(varW->value, varH->value, NULL);
    sv_Init();
    if (!dedicated->value)
        cl_Init();
    hell_Announce("Initialized.\n");
}

void hell_Frame(Tick delta)
{
    hell_i_DrainEvents();
    hell_i_PumpEvents();
    hell_c_Execute();
}

void hell_Loop(void)
{
    const Tick targetFrameLength = (1.0 / vFps->value) * 1000000;
    Tick startTick = hell_Time(); 
    Tick endTick = startTick;
    hell_Announce("Entering Hell Loop.\n");
    while (1)
    {
        // this is just a place holder
        usleep(targetFrameLength - (endTick - startTick));
        endTick = hell_Time();
        hell_Frame(endTick - startTick);
        startTick = endTick;
    }
}

void hell_ShutDown(void)
{
    hell_w_CleanUp();
    hell_i_CleanUp();
    hell_Announce("Shut Down.\n");
}

void hell_Quit(void)
{
    hell_ShutDown();
    exit(0);
}
