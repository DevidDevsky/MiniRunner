CC=gcc
CFLAGS=-Iinclude -Wall -I/opt/homebrew/include -D_THREAD_SAFE
LDFLAGS=$(shell sdl2-config --libs) -lSDL2_image
SRC=$(wildcard src/*.c)
OBJ=$(SRC:.c=.o)
TARGET=runner

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f src/*.o $(TARGET)