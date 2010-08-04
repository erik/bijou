#Uncomment to compile code unoptimized
#no_optimize= true

#Comment to get rid of compatibility flags
compat = true

#Generate debugging symbols
debug = true

CC=gcc
CFLAGS=  -Wall -Wextra -std=c99 $(OPTIMIZE) -lm -DGC
INCS= -Ivm -Ivendor/gc/include -Ivendor
LIBS= ${GC}
GC= vendor/gc/.libs/libgc.a
LDFLAGS= 
SOURCES= vm/vm.c vm/block.c vm/value.c vm/string.c vm/number.c vm/bijou.c
OBJECTS=$(SOURCES:.c=.o)
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

clean:
	@echo " cleaning up"
	@rm -f vm/*.o
	@rm -f vm/*~
	@rm -f $(EXECUTABLE)

rebuild: clean $(EXECUTABLE)


.c.o:
	@echo "   cc $<"
	@${CC} -c ${CFLAGS} ${INCS} $< -o $@
