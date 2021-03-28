hell: error.c error.h cmd.c cmd.h mem.c mem.h com.c com.h main.c
	gcc -Wall error.c error.h cmd.c mem.c com.c main.c -o $@
