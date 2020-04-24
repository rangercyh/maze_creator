all : maze.dll

LUA_INCLUDE = ./include
LUA_LIB = ./ -llua53

CFLAGS = $(CFLAG)
CFLAGS += -g -Wall --shared -I$(LUA_INCLUDE)
LDFLAGS = -L$(LUA_LIB)

maze.dll : maze.c
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean :
	rm maze.dll
