#ifndef HELL_CMD_H
#define HELL_CMD_H

#include <stdint.h>
#include <stdbool.h>

typedef struct Hell_Grimoire Hell_Grimoire;
typedef struct Hell_EventQueue Hell_EventQueue;
typedef void (*Hell_CmdFn)(const Hell_Grimoire*, void* data);

typedef enum {
    HELL_C_VAR_ARCHIVE_BIT = 1 << 0,
} Hell_C_VarFlagBits;

typedef struct Hell_C_Var {
    char*                   name;
    char*                   string;
    char*                   default_string;
    uint32_t                flags;
    bool                    modified;
    double                  value;
    struct Hell_C_Var* next;
} Hell_C_Var;

void  hell_CreateGrimoire(Hell_EventQueue* queue, Hell_Grimoire* grim);
void  hell_DestroyGrimoire(Hell_Grimoire* grim);
void  hell_AddCommand(Hell_Grimoire*, const char* cmdName, Hell_CmdFn, void* data);
void  hell_AddText(Hell_Grimoire*, const char* text);
void  hell_AddNText(Hell_Grimoire*, const char* text, unsigned int len);
void  hell_AddChar(Hell_Grimoire*, const char c);
const char* hell_GetArg(const Hell_Grimoire* grim, unsigned int i);
void  hell_Incantate(Hell_Grimoire*);
void  hell_SetVar(Hell_Grimoire*, const char* name, const char* value,
                    const Hell_C_VarFlagBits flags);
const Hell_C_Var* hell_GetVar(Hell_Grimoire*, const char* name, const char* value, const Hell_C_VarFlagBits flags);

#endif /* end of include guard: HYDROGEN_CMD_H */
