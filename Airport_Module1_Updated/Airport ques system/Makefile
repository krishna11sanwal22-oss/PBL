
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -Isrc
TARGET = airport.exe

SRC = main.c src/queue.c src/algorithms.c src/greedy.c
HEADERS = src/queue.h src/algorithms.h src/greedy.h

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	.\$(TARGET)

clean:
	del /Q $(TARGET) 2>NUL

.PHONY: all run clean
