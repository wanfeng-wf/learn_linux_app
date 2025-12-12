CROSS_COMPILE ?= aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc

SRCS = adc.c

adc_test: $(SRCS)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f adc_test
