CFLAGS := -fPIC -O3 -g -Wall -Werror
CC := gcc

all: libcspec.so cspec example

cspec: cspec.o
	$(CC) $^ -o $@

libcspec.so: cspec_list.o cspec_tree.o cspec.o
	$(CC) -shared $^ -o $@

clean:
	$(RM) *.o *.so

example: libcspec.so cspec_example.so

cspec_example.so: cspec_example.o
	$(CC) -shared $^ -o $@ -L. -lcspec
