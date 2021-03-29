#include <stdio.h>
#include "cmd.h"
#include "input.h"
#include "common.h"

int main(int argc, char *argv[])
{
    hell_Print("Starting.\n");
    hell_i_Init();
    hell_c_Init();
    while (1)
    {
        hell_i_SourceEvents();
        hell_i_DrainEvents();
        hell_c_Execute();
    }
    hell_Print("End.\n");
    return 0;
}
