no_optimize= false

#compatibility flags
compat = true

#Generate debugging symbols
debug = true

#use garbage collector
gc = false

CC=clang
CFLAGS=  -Wall -Wextra -std=c99 $(OPTIMIZE) 
INCS= -Ivm 
LIBS= ${GC}
LDFLAGS= -lm
SOURCES= vm/vm.c vm/block.c vm/value.c vm/string.c vm/number.c vm/compiler.c vm/load.c vm/bijou.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=bijou

ifeq (no_optimize, false)
OPTIMIZE = -O3
endif

ifeq (true, compat)
CFLAGS += -pedantic
endif

ifeq (true, debug)
CFLAGS += -g
endif

ifeq (true, gc)
CFLAGS += -DGC
INCS = -Ivendor/gc/include -Ivendor
GC= vendor/gc/.libs/libgc.a
endif

all:  $(EXECUTABLE)

# count source lines of code
# requires sloccount
sloc: 
	@sloccount vm | grep '(SLOC)'

loc: 
	@cd vm && wc -l *.[ch] vendor/* | grep total

size: 
	@rm -f vm/*~ && du -sh vm

# reformat code (requires astyle)
pretty:
	@echo " formatting"
	@astyle -A4 -n vm/*.[ch] | grep formatted \
		|| echo "   no changes"

# runs valgrind
leaktest: ${EXECUTABLE}
	@valgrind --leak-check=full ./bijou >/dev/null

${EXECUTABLE}: ${LIBS} ${OBJECTS}
	@echo " link $(EXECUTABLE)"
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${OBJECTS} ${LIBS}

${GC}:
	@echo " make gc"
	@cd vendor/gc && ./configure --disable-threads -q && make -s

vm/vm.o:    vm/vm.c vm/bijou.h vm/internal.h vm/bopcodes.h
vm/bijou.o: vm/bijou.c vm/vm.h vm/bijou.h vm/internal.h
vm/block.o: vm/block.c vm/bijou.h vm/internal.h vm/bopcodes.h
vm/string.o: vm/string.c vm/bijou.h vm/internal.h vm/vm.h
vm/number.o: vm/number.c vm/bijou.h vm/internal.h vm/vm.h vm/bopcodes.h
vm/compiler.o: vm/compiler.c vm/bijou.h vm/internal.h
vm/load.o: vm/load.c vm/load.h vm/bijou.h vm/vm.h vm/internal.h vm/compiler.h

clean:
	@echo " cleaning up"
	@rm -f vm/*.o
	@rm -f vm/*~
	@rm -f $(EXECUTABLE)

rebuild: clean $(EXECUTABLE)


.c.o:
	@echo "   cc $<"
	@${CC} -c ${CFLAGS} ${INCS} $< -o $@
