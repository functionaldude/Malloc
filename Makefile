CC = /usr/bin/gcc
CFLAGS  = -w -std=gnu99

all: libmalloc.a

libmalloc.a: malloc.o
	ar rcs libmalloc.a malloc.o

malloc.o: malloc.c
	$(CC) $(CFLAGS) -c malloc.c -o malloc.o

test: unittest
	./unittest

unittest: libmalloc.a
	$(CC) $(CFLAGS) test/test.c -L. -lmalloc -o unittest

clean:
	@rm *o *a unittest