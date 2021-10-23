#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "input.h"
#include "private.h"

typedef Hell_CmdFn CommandFn;

#define MAX_CMD_NAME_LEN 32

typedef uint32_t VarFlags;

typedef struct Cmd {
    struct Cmd* next;
    char        name[MAX_CMD_NAME_LEN];
    void*       data;
    CommandFn   function;
} Cmd;

typedef Hell_C_Var Var;

#define	MAX_CMD_BUFFER	16384
#define	MAX_CMD_LINE	1024

#define	MAX_TOKENS     4	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_SIZE 64

typedef struct Hell_Grimoire {
    Cmd* commands;
    Var* variables;

    char cmdTextBuf[MAX_CMD_BUFFER];
    int  cmdTextCursor;

    int   cmdArgc;
    char* cmdArgv[MAX_TOKENS];
    char  cmdTokenized[MAX_TOKEN_SIZE * MAX_TOKENS];
} Hell_Grimoire;

// so we can rename the struct without having to change the signatures
typedef Hell_Grimoire Grim;

static void cmdListFn(Hell_Grimoire* grim, void* data)
{
    // could filter based on 2nd argument at some point
    // char* match = NULL;
    // if (cmdArgc > 1)
    //     match = cmdArgv[1];

    for (Cmd* cmd = grim->commands; cmd; cmd = cmd->next)
        hell_Print("%s\n", cmd->name);
}

static void cmdEchoFn(Hell_Grimoire* grim, void* data)
{
	for (int i=1 ; i<grim->cmdArgc ; i++)
		hell_Print("%s ", grim->cmdArgv[i]);
	hell_Print("\n");
}

static void varListFn(Hell_Grimoire* grim, void* data)
{
    for (Var* var = grim->variables; var; var = var->next)
    {
        hell_Print("%s: %f\n", var->name, var->value);
    }
}

static void varSetFn(Hell_Grimoire* grim, void* data)
{
}

static void tokenizeString(Grim* grim, const char* text)
{
    assert(text);
    grim->cmdArgc = 0;
    char* textOut = grim->cmdTokenized;
    while ( 1 ) // text will be NUL terminated
    {
        while (*text && *text <= ' ') // skip whitespace
            text++; 
        if (!*text) // all tokens parsed
            return;

        grim->cmdArgv[grim->cmdArgc++] = textOut;
        if (grim->cmdArgc > MAX_TOKENS)
        {
            hell_Print("Too many tokens.\n");
            return;
        }

        while (*text > ' ') // while we are not on whitespace
            *textOut++ = *text++;

        *textOut++ = 0; // end on 0
        assert(textOut - grim->cmdTokenized < MAX_TOKEN_SIZE * MAX_TOKENS);
        if (!*text)
            return;
    }
}

static void execute(Grim* grim, const char* line)
{
    Cmd *cmd, **prev;
    tokenizeString(grim, line);
    if (!grim->cmdArgc)
    {
        return;
    }

	for ( prev = &grim->commands; *prev ; prev = &cmd->next ) 
    {
		cmd = *prev;
		if ( strncmp(grim->cmdArgv[0],cmd->name, MAX_CMD_NAME_LEN) == 0 ) 
        {
			// rearrange the links so that the command will be
			// near the head of the list next time it is used
			*prev = cmd->next;
			cmd->next = grim->commands;
			grim->commands = cmd;

			// perform the action
			if ( !cmd->function ) {
				// let the cgame or game handle it
				break;
			} else {
				cmd->function(grim, cmd->data);
			}
			return;
		}
	}
}

static void cmdInit(Grim* grim)
{
	hell_AddCommand(grim, "cmdlist", cmdListFn, grim);
	hell_AddCommand(grim, "echo",    cmdEchoFn, grim);
}

static void varInit(Grim* grim)
{
    hell_AddCommand(grim, "varlist", varListFn, grim);
	hell_AddCommand(grim, "set", varSetFn, grim);
}

static Var* findVar(const Grim* grim, const char* name)
{
    for (Var* var = grim->variables; var; var = var->next)
    {
        if (strcmp(name, var->name) == 0)
            return var;
    }
    return NULL;
}


static bool consoleEventHandler(const Hell_Event* event, void* pGrimoire)
{
    Hell_Grimoire* grim = (Hell_Grimoire*)pGrimoire;
    hell_AddNText(grim, event->data.conData.ptr,
                    event->data.conData.ptrLen);
    hell_AddChar(grim, '\n');
    return true;
}

const char* hell_GetArg(const Grim* grim, unsigned int i)
{
    if (i >= grim->cmdArgc)
        return ""; // we set to empty string so user can just go straight to strncmp'ing
    return grim->cmdArgv[i];
}

int hell_GetArgC(const Grim* grim)
{
    return grim->cmdArgc;
}

void hell_AddCommand(Hell_Grimoire* grim, const char* cmdName, Hell_CmdFn function, void* data)
{
    assert(strnlen(cmdName, MAX_CMD_NAME_LEN) < MAX_CMD_NAME_LEN);
    Cmd* cmd;
	for (cmd = grim->commands; cmd; cmd = cmd->next)
	{
		if (!strncmp(cmdName, cmd->name, MAX_CMD_NAME_LEN))
		{
			hell_Print("Cmd_AddCommand: %s already defined\n", cmdName);
			return;
		}
	}

    cmd = hell_Malloc(sizeof(Cmd));
    strncpy(cmd->name, cmdName, MAX_CMD_NAME_LEN);
    cmd->function = function;
    cmd->data     = data;

    /* link the command in by name order */
    Cmd** pos = &grim->commands;
    while (*pos && strcmp((*pos)->name, cmdName) < 0)
        pos = &(*pos)->next;
    cmd->next = *pos;
    *pos = cmd;
}

void hell_RemoveCommand(Hell_Grimoire* grim, const char* cmdName)
{
    assert(strnlen(cmdName, MAX_CMD_NAME_LEN) < MAX_CMD_NAME_LEN);
    Cmd *cmd, *prev;
    cmd = grim->commands;
    prev = NULL;
    while (cmd)
    {
        if (strncmp(cmdName, cmd->name, MAX_CMD_NAME_LEN) == 0)
        {
            prev->next = cmd->next;
            hell_Free(cmd);
            return;
        }
        prev = cmd;
        cmd = cmd->next;
    }
}

void hell_AddCommand2(Hell_Grimoire* grim, const char* cmdName, Hell_CmdFn function, void* data, uint32_t dataSize)
{
    assert(strnlen(cmdName, MAX_CMD_NAME_LEN) < MAX_CMD_NAME_LEN);
    Cmd* cmd;
	for (cmd = grim->commands; cmd; cmd = cmd->next)
	{
		if (!strncmp(cmdName, cmd->name, MAX_CMD_NAME_LEN))
		{
			hell_Print("Cmd_AddCommand: %s already defined\n", cmdName);
			return;
		}
	}

    cmd = hell_Malloc(sizeof(Cmd));
    strncpy(cmd->name, cmdName, MAX_CMD_NAME_LEN);
    cmd->function = function;
    cmd->data     = hell_Malloc(dataSize);
    memcpy(cmd->data, data, dataSize);

    /* link the command in by name order */
    Cmd** pos = &grim->commands;
    while (*pos && strcmp((*pos)->name, cmdName) < 0)
        pos = &(*pos)->next;
    cmd->next = *pos;
    *pos = cmd;
}


void hell_SetVar(Hell_Grimoire* grim, const char* name, const char* value, const Hell_C_VarFlagBits flags)
{
    Var* var = findVar(grim, name);

    assert(!var && "Need to handle overwriting the var if it exists.");

    var = hell_Malloc(sizeof(Var));
    var->name = hell_CopyString(name);
    var->string = hell_CopyString(value);
    var->default_string = hell_CopyString(value);
    var->modified = true;
    var->value = strtof(var->string, NULL);

    Var** pos = &grim->variables;
    while (*pos && strcmp((*pos)->name, var->name) < 0)
        pos = &(*pos)->next;
    var->next = *pos;
    *pos = var;

    var->flags = flags;
}

const Var* hell_GetVar(Grim* grim, const char* name, const char* value, const VarFlags flags)
{
    Var* var = findVar(grim, name);
    if (var)
    {
        var->flags |= flags;
        return var;
    }

    var = hell_Malloc(sizeof(Var));
    var->name = hell_CopyString(name);
    var->string = hell_CopyString(value);
    var->default_string = hell_CopyString(value);
    var->modified = true;
    var->value = strtof(var->string, NULL);

    Var** pos = &grim->variables;
    while (*pos && strcmp((*pos)->name, var->name) < 0)
        pos = &(*pos)->next;
    var->next = *pos;
    *pos = var;

    var->flags = flags;
    return var;
}

void hell_AddText(Grim* grim, const char* text)
{
    int l = strnlen(text, MAX_EDIT_LINE);

    if (grim->cmdTextCursor + l >= MAX_CMD_BUFFER)
    {
        hell_Error(0, "Cmd text buffer overflow");
    }
    memcpy(grim->cmdTextBuf + grim->cmdTextCursor, text, l);
    grim->cmdTextCursor += l;
}

void hell_AddNText(Grim* grim, const char* text, unsigned int l)
{
    if (grim->cmdTextCursor + l >= MAX_CMD_BUFFER)
    {
        hell_Error(0, "Cmd text buffer overflow");
    }
    memcpy(grim->cmdTextBuf + grim->cmdTextCursor, text, l);
    grim->cmdTextCursor += l;
}

void hell_AddChar(Grim* grim, const char c)
{
    if (grim->cmdTextCursor + 1 >= MAX_CMD_BUFFER)
        hell_Error(0, "%s cmd text buffer overflow", __FUNCTION__);
    grim->cmdTextBuf[grim->cmdTextCursor++] = c;
}

void hell_CreateGrimoire(Hell_EventQueue* queue, Grim* grim)
{
    memset(grim, 0, sizeof(Hell_Grimoire));
    cmdInit(grim);
    varInit(grim);
    hell_Subscribe(queue, HELL_EVENT_MASK_CONSOLE_BIT, 0, consoleEventHandler, grim);
}

void hell_Incantate(Grim* grim)
{
    int i = 0;
    while (grim->cmdTextCursor)
    {
        char line[MAX_CMD_LINE];
        for (i = 0; i < grim->cmdTextCursor; i++)
        {
            if (grim->cmdTextBuf[i] == '\n')
                break;
        }
        assert(i < MAX_CMD_LINE);
        memcpy(line, grim->cmdTextBuf, i);
        line[i] = 0;

        if (i == grim->cmdTextCursor)
            grim->cmdTextCursor = 0;
        else 
        {
            i++;
            grim->cmdTextCursor -= i;
            memmove(grim->cmdTextBuf, grim->cmdTextBuf + i, grim->cmdTextCursor);
        }

        execute(grim, line);
    }
}

void hell_DestroyGrimoire(Grim* grim)
{
    memset(grim, 0, sizeof(Hell_Grimoire));
}

uint64_t hell_SizeOfGrimoire(void)
{
    return sizeof(Hell_Grimoire);
}

Hell_Grimoire* hell_AllocGrimoire(void)
{
    return hell_Malloc(sizeof(Hell_Grimoire));
}
