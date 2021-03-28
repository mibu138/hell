hell: input.c input.h cmd.c cmd.h mem.c mem.h common.c common.h main.c
	gcc -Wall input.c cmd.c mem.c common.c main.c -o $@
