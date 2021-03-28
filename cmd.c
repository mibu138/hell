#include "mem.h"
#include "cmd.h"
#include "com.h"
#include "error.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>

typedef void (*H_CommandFn)(void);

typedef struct CmdFunction_s {
    struct CmdFunction_s* next;
    char*                 name;
    H_CommandFn           function;
} CmdFunction;

static CmdFunction* cmdFunctions;
static_assert (STDIN_FILENO == 0, "We assume the the fd for stdin if 0");

static void cmdListFn(void)
{
    H_Printf("Command List: \n");
}

static void cmdEchoFn(void)
{
    H_Printf("Echo!\n");
}

static void initConsoleInput(void)
{
    struct termios tc;
    if(isatty(STDIN_FILENO)!=1)
    {
        H_Printf("stdin is not a tty, tty console mode failed");
        H_Abort();
    }
    H_Printf("Started hell console.\n");
    if (tcgetattr(0, &tc) != 0)
        H_ErrorMsg("tcgetattr failed");
    tc.c_lflag &= ~(ECHO | ICANON);
    tc.c_iflag &= ~(ISTRIP | INPCK);
    tc.c_cc[VMIN] = 1;
    tc.c_cc[VTIME] = 0;
    tcsetattr(0, TCSADRAIN, &tc);
}

char* H_ConsoleInput(void)
{
    static char text[256];
    char c;
    int avail = read(0, &c, 1);
    if (avail != -1)
    {
        int fs = sizeof("fucking ");
        int cs = sizeof("cunt ");
        memcpy(text, "fucking ", fs);
        memcpy(text + fs, &c, 1);
        memcpy(text + fs + 1, "cunt ", cs);
        memcpy(text + fs + 1 + cs, " \0", 2);
        write(1, text, fs + cs + 3);
    }
    return text;
}

void H_AddCommand(char* cmdName, H_CommandFn function)
{
    CmdFunction* cmd;
	for (cmd = cmdFunctions; cmd; cmd = cmd->next)
	{
		if (!strcmp(cmdName, cmd->name))
		{
			H_Printf("Cmd_AddCommand: %s already defined\n", cmdName);
			return;
		}
	}

    cmd = H_Malloc(sizeof(CmdFunction));
    cmd->name = cmdName;
    cmd->function = function;

	/* link the command in so that functions stay ordered by name*/
    CmdFunction** pos = &cmdFunctions;
    while (*pos && strcmp((*pos)->name, cmdName) < 0)
        pos = &(*pos)->next;
    cmd->next = *pos;
    *pos = cmd;
}

void H_CmdInit(void)
{
    initConsoleInput();
	H_AddCommand("cmdlist", cmdListFn);
	H_AddCommand("echo",    cmdEchoFn);
}
