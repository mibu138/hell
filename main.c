#include <stdio.h>
#include "cmd.h"
#include "input.h"
#include "common.h"
#include <unistd.h>

static void chungus(void)
{
    hell_Print("Aw lawd %s comin!\n", hell_c_Argv(1));
}

int main(int argc, char *argv[])
{
    hell_Print("Starting.\n");
    hell_i_Init();
    hell_c_Init();
    hell_c_AddCommand("chungus", chungus);
    while (1)
    {
        hell_i_SourceEvents();
        hell_i_DrainEvents();
        hell_c_Execute();
        usleep(16000);
    }
    hell_Print("End.\n");
    return 0;
}
