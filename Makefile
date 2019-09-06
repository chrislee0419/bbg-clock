CC=gcc
CFLAGS=-Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
OBJ=main.o tm1637.o gpio.o
OUTPUT_FILENAME=bbg-clock

all: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_FILENAME)

clean:
	rm $(OUTPUT_FILENAME) *.o
