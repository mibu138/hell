UNIX_LIBS = -lxcb -lxcb-keysyms

hell: display.c display.h input.c input.h cmd.c cmd.h mem.c mem.h common.c common.h main.c
	gcc -Wall display.c input.c cmd.c mem.c common.c main.c -o $@ $(UNIX_LIBS)
