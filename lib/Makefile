CC= gcc
CFLAGS= -Wall -Wextra -std=c99 $(OPTIMIZE) -g -fPIC
INCS= -I../vm
LDFLAGS = -lm -ldl

SOURCES= test.c
OBJECTS= $(SOURCES:.c=.o)

VMSOURCES= ../vm/block.c ../vm/value.c ../vm/string.c ../vm/number.c ../vm/dump.c \
../vm/func.c ../vm/compiler.c ../vm/lib.c ../vm/load.c ../vm/vm.c
VMOBJECTS=$(VMSOURCES:.c=.o)



all: test.so


clean:
	rm -f *.o
	rm -f *.so
	rm -f *~

rebuild: clean all

test.o: test.c
	$(CC) $(CFLAGS) $(INCS) -c -o test.o test.c
test.so: test.o
	$(CC) $(LDFLAGS) -shared -Wl,-soname,test.so  test.o $(VMOBJECTS) -o test.so




