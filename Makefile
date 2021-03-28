hell: cmd.c cmd.h mem.c mem.h com.c com.h main.c
	gcc -Wall cmd.c mem.c com.c main.c -o $@
