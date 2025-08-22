chip8: chip8.c
	cc chip8.c -o chip8

# CC := clang

# CFLAGS := $(shell sdl2-config --cflags) -ggdb3 -O0 -std=c11 -Wall
# LDFLAGS := $(shell sdl2-config --libs)

# SRCS := screen.c

# OBJS := $(SRCS:.c=.o)

# EXEC := screen

# all: $(EXEC)

# $(EXEC): $(OBJS)
# 	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# clean:
# 	rm -f $(OBJS) 