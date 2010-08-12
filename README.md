#Bijou Vitual Machine

A virtual machine, based on Lua and Tinyrb, capable of magic and wonder.

It's a work in progress, and as of now just builds extremely simple things 
already hardcoded into the program.

Originally, I had included the Boehm garbage collector library, but it was gigantic
and Bijou runs without it anyway. If you want to use it, go to
[the download page](http://www.hpl.hp.com/personal/Hans_Boehm/gc/gc_source/),
download it, and place the 'gc' folder inside the vendor dir.

You'll also have to change line 10 of the Makefile to `gc = true`
