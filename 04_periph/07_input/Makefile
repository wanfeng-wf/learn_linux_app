CROSS_COMPILE ?= aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc

input_test: input.c
	$(CC) -o $@ $^

input_poll: input_poll.c
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f input_test
