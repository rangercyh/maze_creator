.PHONY: win linux clean

win : maze.dll
linux: maze.so

LUA_INCLUDE = ./include
LUA_LIB = ./ -llua53

CFLAGS = $(CFLAG)
CFLAGS += -g -std=c99 -Wall -fPIC --shared -I$(LUA_INCLUDE)
LDFLAGS = -L$(LUA_LIB)

maze.dll : maze.c
	gcc $(CFLAGS) -o $@ $^ $(LDFLAGS)

maze.so : maze.c
	gcc $(CFLAGS) -o $@ $^

clean :
	rm maze.dll maze.so
