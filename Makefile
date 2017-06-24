default: test_input

test_input: libinput.a test_input.c
	gcc test_input.c libinput.a -o test_input

libinput.a: libinput.o
	ar -rcv $@ $^
