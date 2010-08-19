#Bijou Virtual Machine

A virtual machine, based on Lua and Tinyrb, capable of magic and wonder.

It's a work in progress, and as of now has it's own (bad) assembly dialect, which
it can compile to bytecode and run (fairly quickly too!)

To try it out, build it, then compile one of the files in 'sample' (I'd suggest
not running loop.s though ;) with `./bijouc sample/<file>.s` and then `./bijou b.out`

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

     Bijou is free software: you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation, either version 3 of the License, or
     (at your option) any later version. 
     
     Bijou is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License, located in the file 'COPYING' for more details.

kvec.h is (c) 2008 Attractive Chaos <http://attractivechaos.awardspace.com/kvec.h.html>
