CC = gcc

TARGET = main
OBJS = main.o hello.o

$(TARGET): $(OBJS)
	cd out && $(CC) -o $@ $^

%.o: %.c
	if [ ! -d out ]; then mkdir -p out; fi && $(CC) -c -o out/$@ $<

.PHONY: clean
clean:
	rm -rf out