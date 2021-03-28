#include <stdio.h>
#include "cmd.h"
#include "common.h"

int main(int argc, char *argv[])
{
    hell_Print("Starting.\n");
    H_CmdInit();
    void bugger(char*);
    while (1)
    {
        const char* input = H_ConsoleInput();
        if (input)
        {
            hell_Print("Hellmouth is open: %s\n", input);
        }
    }
    hell_Print("End.\n");
    return 0;
}
