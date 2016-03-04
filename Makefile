CFLAGS := -fPIC -O3 -Wall -Werror
CC := gcc

all: libcspec.so cspec example

cspec: cspec.o
	$(CC) $^ -o $@ -ldl -L. -lcspec

libcspec.so: cspec_list.o cspec_tree.o cspec.o
	$(CC) -shared $^ -o $@

example: libcspec.so cspec_example.so

cspec_example.so: cspec_example.o
	$(CC) -shared $^ -o $@ -L. -lcspec

clean:
	$(RM) *.o *.so cspec

