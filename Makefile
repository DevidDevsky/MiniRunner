CC=gcc
CFLAGS=-Iinclude -Wall -I/opt/homebrew/include -D_THREAD_SAFE
LDFLAGS=$(shell sdl2-config --libs)

# Common sources (exclude mains)
SRC_COMMON=$(filter-out src/main.c src/demo_main.c, $(wildcard src/*.c))
OBJ_COMMON=$(SRC_COMMON:.c=.o)

# Mains
MAIN_OBJ=src/main.o
DEMO_MAIN_OBJ=src/demo_main.o

TARGET=runner
DEMO_TARGET=runner_demo

all: $(TARGET)

$(TARGET): $(OBJ_COMMON) $(MAIN_OBJ)
	$(CC) $(OBJ_COMMON) $(MAIN_OBJ) -o $@ $(LDFLAGS)

$(DEMO_TARGET): $(OBJ_COMMON) $(DEMO_MAIN_OBJ)
	$(CC) $(OBJ_COMMON) $(DEMO_MAIN_OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f src/*.o $(TARGET) $(DEMO_TARGET)