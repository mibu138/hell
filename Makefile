ifeq ($(OS), Windows_NT)
	OS = WIN
else 
	OS = UNIX
endif

CC        = gcc
NAME      = hell
CFLAGS    = -Wall -fPIC
O         = build
H         = .
LIBS      = 
UNIX_LIBS = -lxcb -lxcb-keysyms
WIN_LIBS  = 
WIN_HEADERS = $(H)/win32_window.h
UNIX_HEADERS = $(H)/unix_window.h
ifeq ($(OS), WIN)
	OS_HEADERS = $(WIN_HEADERS)
	LIBEXT = dll
	LIBS += $(WIN_LIBS)
	HOMEDIR =  "$(HOMEDRIVE)/$(HOMEPATH)"
else
	OS_HEADERS = $(UNIX_HEADERS)
	LIBEXT = so
	LIBS += $(UNIX_LIBS)
	HOMEDIR =  $(HOME)
endif
LIBDIR    = $(HOMEDIR)/lib
LIBNAME   = lib$(NAME).$(LIBEXT)
LIBPATH   = $(LIBDIR)/$(LIBNAME)
TESTNAME  = $(NAME)test

HEADERS = $(H)/display.h $(H)/input.h $(H)/cmd.h $(H)/mem.h $(H)/common.h $(OS_HEADERS)
OBJS    = $(O)/display.o $(O)/input.o $(O)/cmd.o $(O)/mem.o $(O)/common.o 

all: test lib

test: main.c $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $< -o $(TESTNAME) $(LIBS)

lib: $(OBJS)
	$(CC) -shared -o $(LIBPATH) $(OBJS) $(LIBS)

$(O)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(O)/* $(LIBPATH) $(TESTNAME)