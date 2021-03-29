#include "cmd.h"
#include "mem.h"
#include "common.h"
#include "error.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "private.h"

typedef void (*H_CommandFn)(void);

#define MAX_CMD_NAME_LEN 32

typedef struct CmdFunction_s {
    struct CmdFunction_s* next;
    char                  name[MAX_CMD_NAME_LEN];
    H_CommandFn           function;
} CmdFunction;

static CmdFunction* cmdFunctions;

#define	MAX_CMD_BUFFER	16384
#define	MAX_CMD_LINE	1024

#define	MAX_TOKENS     4	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_SIZE 64

static char cmdTextBuf[MAX_CMD_BUFFER];
static int  cmdTextCursor;

static int   cmdArgc;
static char* cmdArgv[MAX_TOKENS];
static char  cmdTokenized[MAX_TOKEN_SIZE * MAX_TOKENS];

static void cmdListFn(void)
{
    // could filter based on 2nd argument at some point
    // char* match = NULL;
    // if (cmdArgc > 1)
    //     match = cmdArgv[1];

    for (CmdFunction* cmd = cmdFunctions; cmd; cmd = cmd->next)
        hell_Print("%s\n", cmd->name);
}

static void cmdEchoFn(void)
{
	for (int i=1 ; i<cmdArgc ; i++)
		hell_Print("%s ", cmdArgv[i]);
	hell_Print("\n");
}

static void tokenizeString(const char* text)
{
    assert(text);
    cmdArgc = 0;
    char* textOut = cmdTokenized;
    while ( 1 ) // text will be NUL terminated
    {
        while (*text && *text <= ' ') // skip whitespace
            text++; 
        if (!*text) // all tokens parsed
            return;

        cmdArgv[cmdArgc++] = textOut;
        if (cmdArgc > MAX_TOKENS)
        {
            hell_Print("Too many tokens.\n");
            return;
        }

        while (*text > ' ') // while we are not on whitespace
            *textOut++ = *text++;

        *textOut++ = 0; // end on 0
        assert(textOut - cmdTokenized < MAX_TOKEN_SIZE * MAX_TOKENS);
        if (!*text)
            return;
    }
}

static void execute(const char* line)
{
    CmdFunction *cmd, **prev;
    tokenizeString(line);
    if (!cmdArgc)
    {
        return;
    }

	for ( prev = &cmdFunctions ; *prev ; prev = &cmd->next ) 
    {
		cmd = *prev;
		if ( strncmp(cmdArgv[0],cmd->name, MAX_CMD_NAME_LEN) == 0 ) 
        {
			// rearrange the links so that the command will be
			// near the head of the list next time it is used
			*prev = cmd->next;
			cmd->next = cmdFunctions;
			cmdFunctions = cmd;

			// perform the action
			if ( !cmd->function ) {
				// let the cgame or game handle it
				break;
			} else {
				cmd->function();
			}
			return;
		}
	}
}

char* hell_c_Argv(unsigned int i)
{
    if (i >= cmdArgc)
        return "";
    return cmdArgv[i];
}

void hell_c_AddCommand(char* cmdName, Hell_C_CommandFn function)
{
    assert(strnlen(cmdName, MAX_CMD_NAME_LEN) < MAX_CMD_NAME_LEN);
    CmdFunction* cmd;
	for (cmd = cmdFunctions; cmd; cmd = cmd->next)
	{
		if (!strncmp(cmdName, cmd->name, MAX_CMD_NAME_LEN))
		{
			hell_Print("Cmd_AddCommand: %s already defined\n", cmdName);
			return;
		}
	}

    cmd = hell_m_Alloc(sizeof(CmdFunction));
    strncpy(cmd->name, cmdName, MAX_CMD_NAME_LEN);
    cmd->function = function;

	/* link the command in by name order */
    CmdFunction** pos = &cmdFunctions;
    while (*pos && strcmp((*pos)->name, cmdName) < 0)
        pos = &(*pos)->next;
    cmd->next = *pos;
    *pos = cmd;
}

void hell_c_AddText(const char* text)
{
    int l = strnlen(text, MAX_EDIT_LINE);

    if (cmdTextCursor + l >= MAX_CMD_BUFFER)
    {
        hell_Error(0, "Cmd text buffer overflow");
    }
    memcpy(cmdTextBuf + cmdTextCursor, text, l);
    cmdTextCursor += l;
}

void hell_c_AddNText(const char* text, unsigned int l)
{
    if (cmdTextCursor + l >= MAX_CMD_BUFFER)
    {
        hell_Error(0, "Cmd text buffer overflow");
    }
    memcpy(cmdTextBuf + cmdTextCursor, text, l);
    cmdTextCursor += l;
}

void hell_c_AddChar(const char c)
{
    if (cmdTextCursor + 1 >= MAX_CMD_BUFFER)
        hell_Error(0, "%s cmd text buffer overflow", __FUNCTION__);
    cmdTextBuf[cmdTextCursor++] = c;
}

void hell_c_Init(void)
{
	hell_c_AddCommand("cmdlist", cmdListFn);
	hell_c_AddCommand("echo",    cmdEchoFn);
}

void hell_c_Execute(void)
{
    int i = 0;
    while (cmdTextCursor)
    {
        char line[MAX_CMD_LINE];
        for (i = 0; i < cmdTextCursor; i++)
        {
            if (cmdTextBuf[i] == '\n')
                break;
        }
        assert(i < MAX_CMD_LINE);
        memcpy(line, cmdTextBuf, i);
        line[i] = 0;

        if (i == cmdTextCursor)
            cmdTextCursor = 0;
        else 
        {
            i++;
            cmdTextCursor -= i;
            memmove(cmdTextBuf, cmdTextBuf + i, cmdTextCursor);
        }

        execute(line);
    }
}
