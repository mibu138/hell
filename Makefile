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
WIN_HEADERS = $(H)/win32_window.h
UNIX_HEADERS = $(H)/unix_window.h
ifeq ($(OS), WIN)
	OS_HEADERS = $(WIN_HEADERS)
	LIBEXT = dll
	XEXT = .exe
	HOMEDIR = C:
else
	OS_HEADERS = $(UNIX_HEADERS)
	LIBEXT = so
	LIBS += -lxcb -lxcb-keysyms -ldl
	HOMEDIR =  $(HOME)
endif
LIBDIR    = $(HOMEDIR)/lib
LIBNAME   = lib$(NAME).$(LIBEXT)
LIBPATH   = $(LIBDIR)/$(LIBNAME)
TESTNAME  = $(NAME)test$(XEXT)

HEADERS = $(H)/display.h $(H)/input.h $(H)/cmd.h $(H)/mem.h $(H)/common.h $(OS_HEADERS)
OBJS    = $(O)/display.o $(O)/input.o $(O)/cmd.o $(O)/mem.o $(O)/common.o 

all: lib test 

test: main.c $(OBJS)
	$(CC) $(CFLAGS) -L$(LIBDIR) $< -o $(TESTNAME) -l$(NAME)

lib: $(OBJS)
	$(CC) -shared -o $(LIBPATH) $(OBJS) $(LIBS)

$(O)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(O)/* $(LIBPATH) $(TESTNAME)
