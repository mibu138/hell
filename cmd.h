#ifndef HELL_CMD_H
#define HELL_CMD_H

typedef void (*H_CommandFn)(void);

void  H_CmdInit(void);
void  H_AddCommand(char* cmdName, H_CommandFn fn);
char* H_ConsoleInput(void);


#endif /* end of include guard: HYDROGEN_CMD_H */
