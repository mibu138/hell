#ifndef HELL_CMD_H
#define HELL_CMD_H

typedef void (*Hell_C_CommandFn)(void);

void  hell_c_Init(void);
void  hell_c_AddCommand(char* cmdName, Hell_C_CommandFn fn);
void  hell_c_AddText(const char* text);
void  hell_c_AddNText(const char* text, unsigned int len);
void  hell_c_AddChar(const char c);
char* hell_c_Argv(unsigned int i);
void  hell_c_Execute(void);

#endif /* end of include guard: HYDROGEN_CMD_H */
