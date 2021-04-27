#ifndef HELL_CMD_H
#define HELL_CMD_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*Hell_C_CmdFn)(void);

typedef enum {
    HELL_C_VAR_ARCHIVE_BIT = 1 << 0,
} Hell_C_VarFlagBits;

typedef struct Hell_C_Var {
    char*                   name;
    char*                   string;
    char*                   default_string;
    uint32_t                flags;
    bool                    modified;
    float                   value;
    struct Hell_C_Var* next;
} Hell_C_Var;

void              hell_c_Init(void);
void              hell_c_AddCommand(const char* cmdName, Hell_C_CmdFn fn);
void              hell_c_AddText(const char* text);
void              hell_c_AddNText(const char* text, unsigned int len);
void              hell_c_AddChar(const char c);
char*             hell_c_Argv(unsigned int i);
void              hell_c_Execute(void);
void              hell_c_SetVar(const char* name, const char* value, const Hell_C_VarFlagBits flags);
const Hell_C_Var* hell_c_GetVar(const char* name, const char* value, const Hell_C_VarFlagBits flags);

#endif /* end of include guard: HYDROGEN_CMD_H */
