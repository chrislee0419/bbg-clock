CROSS=arm-linux-gnueabihf-
CC=$(CROSS)gcc
CFLAGS=-Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
OBJ=main.o tm1637.o gpio.o
OUTPUT_FILENAME=bbg-clock

SSH_USER=root
BBG_ADDRESS=192.168.100.2
BBG_DEST=/root/bbg-clock

all: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $(OUTPUT_FILENAME)
	scp $(OUTPUT_FILENAME) $(SSH_USER)@$(BBG_ADDRESS):$(BBG_DEST)

run:
	ssh $(SSH_USER)@$(BBG_ADDRESS) "/root/bbg-clock/bbg-clock"

clean:
	rm $(OUTPUT_FILENAME) *.o
