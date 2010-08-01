#Uncomment to compile code unoptimized
#no_optimize= true

#Comment to get rid of compatibility flags
compat = true

#Generate debugging symbols
debug = true

CC=gcc
CFLAGS=  -Wall -Wextra -Werror -std=c99 $(OPTIMIZE)
INCS= -Ivm -Ivendor/gc/include -Ivendor
LIBS= ${GC}
GC= vendor/gc/.libs/libgc.a
LDFLAGS=
SOURCES= vm/vm.c vm/bijou.c
OBJECTS=$(SOURCES:.c=.o)
#OBJECTS= vm/vm.o vm/bijou.o
EXECUTABLE=bijou


ifndef no_optimize
OPTIMIZE = -O3
endif

ifdef compat
CFLAGS += -pedantic
endif

ifdef debug
CFLAGS += -g
endif

all:  $(EXECUTABLE)

tmp:
	@echo $(OBJECTS)

sloc:
	@sloccount vm
depend:
	$(CC) $(CFLAGS) $(INCS) -MM $(SOURCES)
o: $(OBJECTS)

# reformat code (requires astyle)
pretty:
	@echo " Formatting"
	@astyle -A8 $(SOURCES)
	@astyle -A8 vm/vendor/*.h
	@rm -f vm/*.orig vm/vendor/*.orig

${EXECUTABLE}: ${LIBS} ${OBJECTS}
	@echo " link $(EXECUTABLE)"
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${OBJECTS} ${LIBS}

${GC}:
	@echo " make gc"
	@cd vendor/gc && ./configure --disable-threads -q && make -s

vm/vm.o:   vm/vm.c vm/bijou.h vm/internal.h vm/bopcodes.h
vm/bijou.o: vm/bijou.c vm/vm.h vm/bijou.h vm/internal.h


clean: 
	rm -f vm/*.o
	rm -f $(EXECUTABLE)

rebuild: clean $(EXECUTABLE)


.c.o:
	@echo "   cc $<"
	@${CC} -c ${CFLAGS} ${INCS} $< -o $@
