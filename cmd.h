#ifndef HELL_CMD_H
#define HELL_CMD_H

typedef void (*Hell_C_CommandFn)(void);

void  hell_c_Init(void);
void  hell_c_AddCommand(char* cmdName, Hell_C_CommandFn fn);
void  hell_c_BufExecute(void);

#endif /* end of include guard: HYDROGEN_CMD_H */
