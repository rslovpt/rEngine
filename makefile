CC = gcc
CFLAGS = -Isrc/include/SDL2 -Isrc/include/scripts
LDFLAGS = -Lsrc/lib -lSDL2
TARGET = main

SRC = $(wildcard src/*.c) $(wildcard src/include/scripts/*.c) $(wildcard src/include/scripts/file/*.c) $(wildcard src/include/scripts/matrix/*.c) $(wildcard src/include/scripts/camera/*.c)

all: $(TARGET) postbuild

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

postbuild:
	copy src\lib\SDL2.dll .\ 2> NUL

clean:
	del /Q $(TARGET).exe 2> NUL