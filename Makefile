CC=GCC
CFLAGS=-Wall -Wextra

#Change the name of the main file here, and the name of the desired executable file
MAINNAME = test.c
OUTNAME = test


default: $(OUTNAME) setup

$(OUTNAME): $(MAINNAME) libtalaris.so
	gcc $(MAINNAME) -ltalaris -o  $@ -Wall -Wextra

libtalaris.a: libtalaris.o
	ar -rcv $@ $^

libtalaris.o: libtalaris.c libtalaris.h
	gcc -c -fPIC libtalaris.c -o $@

libtalaris.so: libtalaris.o
	gcc -shared -o libtalaris.so libtalaris.o

setup:
	sudo cp ./libtalaris.so /usr/lib
	sudo chmod 0755 /usr/lib/libtalaris.so
	sudo ldconfig
