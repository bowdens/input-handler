CC=GCC
CFLAGS=-Wall -Wextra -std=gnu11

#Change the name of the main file here, and the name of the desired executable file
MAINNAME = test.c
OUTNAME = test


default: setup

$(OUTNAME): libtalaris.so $(MAINNAME)
	gcc $(MAINNAME) -ltalaris -o  $@ -Wall -Wextra

libtalaris.so: libtalaris.o
	gcc -shared -o libtalaris.so libtalaris.o
	make setup

libtalaris.a: libtalaris.o
	ar -rcv $@ $^

libtalaris.o: libtalaris.c libtalaris.h
	gcc -c -fPIC libtalaris.c -o $@

setup: libtalaris.so
	sudo cp ./libtalaris.so /usr/lib
	sudo chmod 0755 /usr/lib/libtalaris.so
	sudo ldconfig
	sudo cp ./libtalaris.h /usr/include/
