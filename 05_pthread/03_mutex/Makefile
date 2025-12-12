REMOTE_HOST = root@192.168.7.2
TARGET_PATH = $(REMOTE_HOST):/root/thread_mutex

CROSS_COMPILE ?= aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc

all: main

main: main.c
	$(CC) -o $@ $^ -lpthread

push: $(TARGET)
	scp ./main $(TARGET_PATH)

.PHONY: clean

clean:
	rm -f main
