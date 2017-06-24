default: test_input #test_input_static test_input_dynamic

test_input_static: test_input.c libinput.a
	gcc $^ -o $@

test_input: test_input.c libinput.so
	gcc test_input.c -L. -linput -o  $@

libinput.a: libinput.o
	ar -rcv $@ $^

libinput.so: libinput.o
	gcc -shared -o libinput.so libinput.o

libinput.o: libinput.c libinput.h
	gcc -c -fPIC libinput.c -o $@
