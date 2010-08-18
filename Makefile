no_optimize= false

#compatibility flags
compat = true

#Generate debugging symbols
debug = true

#use garbage collector
gc = false

CC=gcc
CFLAGS=  -Wall -Wextra -std=c99 $(OPTIMIZE) 
INCS= -Ivm 
LIBS= ${GC}
LDFLAGS= -lm

ALLSOURCES= vm/block.c vm/value.c vm/string.c vm/number.c vm/dump.c vm/func.c \
vm/compiler.c vm/lib.c vm/load.c vm/vm.c
ALLOBJECTS=$(ALLSOURCES:.c=.o)

VMSOURCES= $(ALLSOURCES) vm/bijou.c
VMOBJECTS= $(VMSOURCES:.c=.o)

COMPILERSOURCES= $(ALLSOURCES) vm/bijouc.c
COMPILEROBJECTS= $(COMPILERSOURCES:.c=.o)

VM=bijou
COMPILER=bijouc

ifeq ($(no_optimize), false)
OPTIMIZE = -O3
endif

ifeq ($(compat), true)
CFLAGS += -pedantic
endif

ifeq ($(debug), true)
CFLAGS += -g 
endif

ifeq ($(gc), true)
CFLAGS += -DGC
INCS = -Ivendor/gc/include -Ivendor
GC= vendor/gc/.libs/libgc.a
endif

all:  $(VM) $(COMPILER)

t:
	@echo $(VMSOURCES)
	@echo $(VMOBJECTS)
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
leaktest: ${VM} ${COMPILER}
	valgrind -q --leak-check=full --show-reachable=yes ./bijouc sample/test.s -o val.out
	valgrind -q --leak-check=full --show-reachable=yes ./bijou val.out

test: ${VM} ${COMPILER}
	./bijouc sample/test.s -o val.out
	./bijou val.out
	@rm -f val.out

${VM}: ${LIBS} ${VMOBJECTS}
	@echo " link $(VM)"
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${VMOBJECTS} ${LIBS}

${COMPILER}: ${LIBS} ${COMPILEROBJECTS}
	@echo " link $(COMPILER)"
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${COMPILEROBJECTS} ${LIBS}

${GC}:
	@echo " make gc"
	@cd vendor/gc && ./configure --disable-threads -q && make -s

vm/bijou.o:    	vm/bijou.c vm/vm.h vm/bijou.h vm/internal.h vm/bopcodes.h \
vm/dump.h vm/load.h
vm/bijouc.o:   	vm/bijouc.c vm/bijouc.h vm/internal.h vm/bijou.h vm/compiler.h
vm/block.o: 	vm/block.c vm/bopcodes.h vm/internal.h vm/dump.h vm/bijou.h
vm/compiler.o: 	vm/compiler.c vm/compiler.h vm/bijou.h vm/vm.h vm/bopcodes.h \
vm/dump.h vm/bijouc.h
vm/dump.o: 	vm/dump.c vm/config.h vm/bijou.h vm/internal.h vm/dump.h \
vm/bopcodes.h
vm/func.o: 	vm/func.c vm/bijou.h vm/func.h
vm/lib.o:	vm/lib.c vm/lib.h vm/bijou.h vm/internal.h vm/vm.h vm/func.h
vm/load.o: 	vm/load.c vm/bijou.h vm/load.h vm/vm.h vm/internal.h vm/dump.h
vm/number.o: 	vm/number.c vm/bijou.h vm/internal.h vm/vm.h vm/bopcodes.h
vm/string.o: 	vm/string.c vm/bijou.h vm/internal.h vm/vm.h
vm/value.o:	vm/value.c vm/bijou.h vm/internal.h
vm/vm.o:    	vm/vm.c vm/bijou.h vm/internal.h vm/bopcodes.h vm/vm.h \
vm/lib.h vm/func.h


clean:
	@echo " cleaning up"
	@rm -f vm/*.o
	@rm -f vm/*~
	@rm -f $(VM)
	@rm -f $(COMPILER)

rebuild: clean $(VM) $(COMPILER)


.c.o:
	@echo "   cc $<"
	@${CC} -c ${CFLAGS} ${INCS} $< -o $@
