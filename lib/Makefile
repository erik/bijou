CC= gcc
CFLAGS= -Wall -Wextra -std=c99 $(OPTIMIZE) -g -fPIC
INCS= -I../vm
LDFLAGS = -lm -ldl -shared
SOURCES= test.c
OBJECTS= $(SOURCES:.c=.o)

#shared object extension
SOEXT=so

LIBS= $(OBJECTS:.o=.$(SOEXT))

VMSOURCES= ../vm/block.c ../vm/value.c ../vm/string.c ../vm/number.c ../vm/dump.c \
../vm/func.c ../vm/compiler.c ../vm/lib.c ../vm/load.c ../vm/vm.c
VMOBJECTS=$(VMSOURCES:.c=.o)

# Macs use .dylib instead of .so
ifeq ("$(shell uname -s)", "Darwin")
SOEXT=dylib
LDFLAGS +=-Wl-dylib
else
LDFLAGS +=-Wl,-soname,$@
endif



all: $(SOURCES) $(OBJECTS) $(LIBS)


clean:
	rm -f *.o
	rm -f *.$(SOEXT)
	rm -f *~

rebuild: clean all

test.o: test.c

test.$(SOEXT): test.o
	@echo "  link test"
	@$(CC) $(LDFLAGS)  $< $(VMOBJECTS) -o $@

.c.o:
	@echo "  cc $<"
	@$(CC) $(CFLAGS) $(INCS) $< -c -o $@



