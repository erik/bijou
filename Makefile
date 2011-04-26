no_optimize= true

#compatibility flags
compat = true

#Generate debugging symbols
debug = true

#use garbage collector
gc = false

CC=clang
CFLAGS=  -Wall -Wextra -pedantic -std=c99 -fPIC $(OPTIMIZE) 
INCS= -Isrc
LIBS= ${GC}
LDFLAGS= -lm -ldl -lpthread

ALLSOURCES= src/block.c src/value.c src/string.c src/number.c src/dump.c \
src/func.c src/compiler.c src/lib.c src/load.c src/vm.c src/const.c
ALLOBJECTS=$(ALLSOURCES:.c=.o)

VMSOURCES= $(ALLSOURCES) src/bijou.c
VMOBJECTS= $(VMSOURCES:.c=.o)

COMPILERSOURCES= $(ALLSOURCES) src/bijouc.c
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

all: $(VM) $(COMPILER) bijoulib 

# count source lines of code
# requires sloccount
sloc: 
	@sloccount src lib | grep '(SLOC)'

loc: 
	@wc -l src/*.[ch] src/vendor/* lib/*.[ch] | grep total

size: 
	@rm -f src/*~ lib/*~ && du -sh src lib

todo:
	@find src lib -type f | xargs grep -n -i "TODO"
	@find src lib -type f | xargs grep -n -i "FIXME"

# reformat code (requires astyle)
pretty:
	@echo " formatting"
	@astyle -A4 -n src/*.[ch] lib/*.[ch] | grep formatted \
		|| echo "   no changes"

# runs valgrind
leaktest: ${VM} ${COMPILER}
	valgrind -q --leak-check=full --show-reachable=yes ./bijouc sample/test.s -o val.out
	valgrind -q --leak-check=full --show-reachable=yes ./bijou val.out

test: ${VM} ${COMPILER}
	./bijouc sample/test.s -o val.out
	./bijou val.out
	./bijouc sample/call.s -o val.out
	./bijou val.out
	./bijouc sample/function.s -o val.out
	./bijou val.out
	./bijouc sample/string.s -o val.out
	./bijou -l lib/string.so val.out
	@rm -f val.out

bijoulib: $(wildcard lib/*.so)
	@cd lib && make

${VM}: ${LIBS} ${VMOBJECTS}
	@echo " link $(VM)"
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${VMOBJECTS} ${LIBS}

${COMPILER}: ${LIBS} ${COMPILEROBJECTS}
	@echo " link $(COMPILER)"
	@${CC} -o $@ ${CFLAGS} ${LDFLAGS} ${COMPILEROBJECTS} ${LIBS}

${GC}:
	@echo " make gc"
	@cd vendor/gc && ./configure --disable-threads -q && make -s

src/bijou.o:    src/bijou.c src/vm.h src/bijou.h src/internal.h src/bopcodes.h \
src/dump.h src/load.h
src/bijouc.o:   src/bijouc.c src/bijouc.h src/internal.h src/bijou.h src/compiler.h
src/block.o: 	src/block.c src/bopcodes.h src/internal.h src/dump.h src/bijou.h \
src/vm.h
src/compiler.o: src/compiler.c src/compiler.h src/bijou.h src/vm.h src/bopcodes.h \
src/dump.h src/bijouc.h
src/const.o:	src/const.c src/const.h src/bijou.h src/vm.h src/internal.h	
src/dump.o: 	src/dump.c src/config.h src/bijou.h src/internal.h src/dump.h \
src/bopcodes.h
src/func.o: 	src/func.c src/bijou.h src/func.h src/vm.h src/internal.h
src/lib.o:	src/lib.c src/lib.h src/bijou.h src/internal.h src/vm.h src/func.h
src/load.o: 	src/load.c src/bijou.h src/load.h src/vm.h src/internal.h src/dump.h
src/number.o: 	src/number.c src/bijou.h src/internal.h src/vm.h src/bopcodes.h
src/string.o: 	src/string.c src/bijou.h src/internal.h src/vm.h
src/value.o:	src/value.c src/bijou.h src/internal.h
src/vm.o:    	src/vm.c src/bijou.h src/internal.h src/bopcodes.h src/vm.h \
src/lib.h src/func.h


clean:
	@echo " cleaning up"
	@rm -f src/*.o
	@rm -f src/*~
	@rm -f $(VM)
	@rm -f $(COMPILER)

distclean: clean
	@cd lib && make clean

completeclean: clean distclean
	@cd vendor/gc && make clean

rebuild: clean $(VM) $(COMPILER) bijoulib


.c.o:
	@echo "   cc $<"
	@${CC} -c ${CFLAGS} ${INCS} $< -o $@
