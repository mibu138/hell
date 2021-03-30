CC        = gcc
NAME      = hell
CFLAGS    = -Wall -fPIC
UNIX_LIBS = -lxcb -lxcb-keysyms
O         = build
H         = .
LIBDIR    = $(HOME)/lib
LIBNAME   = lib$(NAME).so
LIBPATH   = $(LIBDIR)/$(LIBNAME)
TESTNAME  = $(NAME)test

HEADERS = $(H)/display.h $(H)/input.h $(H)/cmd.h $(H)/mem.h $(H)/common.h
OBJS    = $(O)/display.o $(O)/input.o $(O)/cmd.o $(O)/mem.o $(O)/common.o

all: test lib

test: main.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $(TESTNAME) $(UNIX_LIBS)

lib: $(OBJS)
	$(CC) -shared -o $(LIBPATH) $(OBJS) $(UNIX_LIBS)

$(O)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(O)/* $(LIBPATH) $(TESTNAME)
