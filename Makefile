CC        = gcc
NAME      = hell
CFLAGS    = -Wall -fPIC
UNIX_LIBS = -lxcb -lxcb-keysyms
O         = build
H         = .
LIBDIR    = $(HOME)/lib
LIBNAME   = $(NAME).so
LIBPATH   = $(LIBDIR)/$(LIBNAME)

HEADERS = $(H)/display.h $(H)/input.h $(H)/cmd.h $(H)/mem.h $(H)/common.h
OBJS    = $(O)/display.o $(O)/input.o $(O)/cmd.o $(O)/mem.o $(O)/common.o

all: test 

test: main.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $@ $(UNIX_LIBS)

lib: $(OBJS)
	$(CC) -shared -o $(LIBPATH) $(OBJS) $(UNIX_LIBS)

$(O)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(O)/* $(LIBPATH) test
