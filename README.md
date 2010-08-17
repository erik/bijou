#Bijou Virtual Machine

A virtual machine, based on Lua and Tinyrb, capable of magic and wonder.

It's a work in progress, and as of now has it's own (bad) assembly dialect, which
it can compile to bytecode and run (fairly quickly too!)

##Requirements
* GCC or Clang (and probably other standard compilers)
* Make
* Boehm garbage collector (optional -- see below)

Originally, I had included the Boehm garbage collector library, but it was gigantic
and Bijou runs without it anyway. If you want to use it, go to
[the download page](http://www.hpl.hp.com/personal/Hans_Boehm/gc/gc_source/),
download it, and place the 'gc' folder inside the top level vendor directory (not vm/vendor).

You'll also have to change line 10 of the Makefile to `gc = true`

##Installation
1. Clone the repo
2. run `make`
3. run `make test` (optional -- this compiles a simple example program to bytecode and runs it)

##License
Bijou is free software released under the terms of the GNU GPLv3 license. See [here](http://www.gnu.org/licenses/gpl.txt) for a copy.