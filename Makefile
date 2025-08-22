# chip8: chip8.c
# 	cc chip8.c -o chip8

CC := cc
CFLAGS := -Wall -Wextra -std=c11 -g $(shell sdl2-config --cflags)
LDFLAGS := $(shell sdl2-config --libs)

chip8: chip8.c chip8.h screen.h
	$(CC) $(CFLAGS) chip8.c -o chip8 $(LDFLAGS)

clean:
	rm -f chip8
