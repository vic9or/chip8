CFLAGS= -std=c17 -Wall -Werror
all:
	gcc chip8.c -o chip8 $(CFLAGS) `sdl2-config --cflags --libs`
