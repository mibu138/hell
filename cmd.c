#include "cmd.h"
#include "mem.h"
#include "common.h"
#include "error.h"
#include <string.h>
#include <stdio.h>
#include "private.h"

typedef void (*H_CommandFn)(void);

typedef struct CmdFunction_s {
    struct CmdFunction_s* next;
    char*                 name;
    H_CommandFn           function;
} CmdFunction;

static CmdFunction* cmdFunctions;

static void cmdListFn(void)
{
    hell_Print("Command List: \n");
}

static void cmdEchoFn(void)
{
    hell_Print("Echo!\n");
}

void hell_c_AddCommand(char* cmdName, Hell_C_CommandFn function)
{
    CmdFunction* cmd;
	for (cmd = cmdFunctions; cmd; cmd = cmd->next)
	{
		if (!strcmp(cmdName, cmd->name))
		{
			hell_Print("Cmd_AddCommand: %s already defined\n", cmdName);
			return;
		}
	}

    cmd = hell_m_Alloc(sizeof(CmdFunction));
    cmd->name = cmdName;
    cmd->function = function;

	/* link the command in so that functions stay ordered by name*/
    CmdFunction** pos = &cmdFunctions;
    while (*pos && strcmp((*pos)->name, cmdName) < 0)
        pos = &(*pos)->next;
    cmd->next = *pos;
    *pos = cmd;
}

void hell_c_AddText(const char* text)
{
    int l = strnlen(text, MAX_EDIT_LINE);
}

void hell_c_Init(void)
{
	hell_c_AddCommand("cmdlist", cmdListFn);
	hell_c_AddCommand("echo",    cmdEchoFn);
}

void hell_c_Execute(void)
{
}
