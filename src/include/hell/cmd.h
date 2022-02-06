#ifndef HELL_CMD_H
#define HELL_CMD_H

#include <stdbool.h>
#include <stdint.h>

typedef struct Hell_Grimoire   Hell_Grimoire;
typedef struct Hell_EventQueue Hell_EventQueue;
// Grimoire cannot be const because running a command may add/remove another
// command
typedef void (*Hell_CmdFn)(Hell_Grimoire*, void* data);

typedef enum {
    HELL_VAR_ARCHIVE_BIT = 1 << 0,
} Hell_VarFlagBits;

typedef uint32_t Hell_VarFlags;

#ifdef HELL_VAR_VALUE_64_BIT
typedef double Hell_VarValue;
#else
typedef float Hell_VarValue;
#endif

typedef struct Hell_Var {
    char*            name;
    Hell_VarFlags    flags;
    Hell_VarValue    value;
    struct Hell_Var* next;
} Hell_Var;

void        hell_CreateGrimoire(Hell_EventQueue* queue, Hell_Grimoire* grim);
void        hell_DestroyGrimoire(Hell_Grimoire* grim);
void        hell_AddCommand(Hell_Grimoire*, const char* cmdName, Hell_CmdFn,
                            void* data);
void        hell_AddCommand2(Hell_Grimoire* grim, const char* cmdName,
                             Hell_CmdFn function, void* data, uint32_t dataSize);
void        hell_RemoveCommand(Hell_Grimoire* grim, const char* cmdName);
void        hell_AddText(Hell_Grimoire*, const char* text);
void        hell_AddNText(Hell_Grimoire*, const char* text, unsigned int len);
void        hell_AddChar(Hell_Grimoire*, const char c);
const char* hell_GetArg(const Hell_Grimoire* grim, unsigned int i);
int         hell_GetArgC(const Hell_Grimoire* grim);
void        hell_Incantate(Hell_Grimoire*);
void        hell_SetVar(Hell_Grimoire*, const char* name, double value,
                        Hell_VarFlags flags);
Hell_Var* hell_GetVar(Hell_Grimoire*, const char* name, double value,
                            Hell_VarFlags flags);

#endif /* end of include guard: HYDROGEN_CMD_H */
