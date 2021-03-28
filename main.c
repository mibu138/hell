#include <stdio.h>
#include "cmd.h"

int main(int argc, char *argv[])
{
    printf("Starting.\n");
    H_CmdInit();
    H_ConsoleInput();
    printf("End.\n");
    return 0;
}
