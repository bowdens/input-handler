CC=GCC
CFLAGS=-Wall -Wextra

default: test_input_static

test_input_static: test_input.c libtalaris.c libtalaris.h
	gcc -o test_input_static test_input.c libtalaris.c libtalaris.h -Wall -Wextra

test_input: test_input.c libtalaris.so
	gcc test_input.c -L/home/tom/Desktop/code/c/input -ltalaris -o  $@ -Wall -Wextra

libtalaris.a: libtalaris.o
	ar -rcv $@ $^

libtalaris.o: libtalaris.c libtalaris.h
	gcc -c -fPIC libtalaris.c -o $@

libtalaris.so: libtalaris.o
	gcc -shared -o libtalaris.so libtalaris.o
