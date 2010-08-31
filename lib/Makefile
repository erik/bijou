CC= gcc
# the -D_POSIX_C_SOURCE=200112L bit is for compiling net. (don't ask)
CFLAGS= -Wall -Wextra -std=c99 $(OPTIMIZE) -g -fPIC -D_POSIX_C_SOURCE=200112L
INCS= -I../src
LDFLAGS = -lm -ldl -shared
SOURCES= test.c net.c thread.c string.c io.c debug.c
OBJECTS= $(SOURCES:.c=.o)

#shared object extension
SOEXT=so

LIBS= $(OBJECTS:.o=.$(SOEXT))

VMSOURCES= ../src/block.c ../src/value.c ../src/string.c ../src/number.c ../src/dump.c \
../src/func.c ../src/compiler.c ../src/lib.c ../src/load.c ../src/vm.c
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

test.o:   test.c
net.o:    net.c
thread.o: thread.c
string.o: string.c
io.o:     io.c
debug.o:  debug.c

test.$(SOEXT): test.o
	@echo "  link test"
	@$(CC) $(LDFLAGS)  $< $(VMOBJECTS) -o $@
net.$(SOEXT): net.o
	@echo "  link net"
	@$(CC) $(LDFLAGS) $< $(VMOBJECTS) -o $@
thread.$(SOEXT): thread.o
	@echo "  link thread"
	@$(CC) -lpthread $(LDFLAGS) $< $(VMOBJECTS) -o $@
string.$(SOEXT): string.o
	@echo "  link string"
	@$(CC) $(LDFLAGS) $< $(VMOBJECTS) -o $@
io.$(SOEXT): io.o
	@echo "  link io"
	@$(CC) $(LDFLAGS) $< $(VMOBJECTS) -o $@
debug.$(SOEXT): debug.o
	@echo "  link debug"
	@$(CC) $(LDFLAGS) $< $(VMOBJECTS) -o $@

.c.o:
	@echo "  cc $<"
	@$(CC) $(CFLAGS) $(INCS) $< -c -o $@



