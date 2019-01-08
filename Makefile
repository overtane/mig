CFLAGS ?= -O0 -g -Wall
SFLAGS = -Wno-unused-function -Wno-unneeded-internal-declaration
OBJS = mig.o scanner.o parser.o
SRCS = mig.c scanner.c parser.c

.PHONY: all tests

mig: $(OBJS)
	$(CC) $(CFLAGS) -ll -o mig $(OBJS)

mig.o: mig.c mig.h

scanner.o: scanner.c
	$(CC) $(CFLAGS) $(SFLAGS) -c -o $@ $<

scanner.c: scanner.l parser.c
	flex -o scanner.c scanner.l

parser.o: parser.c parser.h mig.h 

parser.c: parser.y mig.h
	bison -d -t -o parser.c parser.y

all: mig tests

tests:
	$(MAKE) -C $@

clean:
	rm parser.h parser.c parser.o
	rm scanner.c scanner.o
	rm mig.o
	rm mig
	$(MAKE) -C tests clean
