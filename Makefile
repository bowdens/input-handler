default: test_input test_input_static

test_input_static: test_input.c libtalaris.a
	gcc $^ -o $@

test_input: test_input.c libtalaris.so
	gcc test_input.c -L. -ltalaris -o  $@ -Wall -Wextra

libtalaris.a: libtalaris.o
	ar -rcv $@ $^

libtalaris.so: libtalaris.o
	gcc -shared -o libtalaris.so libtalaris.o

libtalaris.o: libtalaris.c libtalaris.h
	gcc -c -fPIC libtalaris.c -o $@
