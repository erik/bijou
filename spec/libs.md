#Dynamically Loaded Libraries

Bijou has the ability to load libraries written in C (or feasibly any natively
compiled language) at runtime.

Advantages:

  - Small core library
  - Easy to write
  - Simple way to extend Bijou's functionality
  - Functions are fast. 

Disadvantages:

  - Non-portable across operating systems (the code itself should be portable however)
  - Increased load time
  - Some errors that would be caught at link time slip through to runtime

To create a library, all you need to do is mind your function naming and supply
some different flags to your compiler (see lib/Makefile for Linux/Mac compilation).

The opcode 'getexternal' places into R[A] the function named by constant B. So,
for instance:
        >CODE
            getexternal 0 0 ; R[0] = closure("a_func")
        <CODE
        >CONST
            3#"a_func"
        <CONST

Will look for the function "a_func" from the included loaded libs, and create a
closure out of it, which is placed in R[0]

R[0] can then be `call`'d in the same way as regular closures or functions

If you then wanted to define 'a_func' in C, you would do this:
        int    args_a_func = 0; /* a_func takes 0 args */
        TValue func_a_func(BijouVM* vm, BijouBlock* blk, int argc, TValue* argv)
        {
            /* function definition */
        }

For any function X you want to call in Bijou, you'll need an int: `args_X` to 
specify the number of arguments the function takes (-1 for any amount), and to
declare the function as `func_X` taking the arguments listed above.

The definitions for the various structs can be found in vm/vm.h and vm/bijou.h.
You probably won't need anything but argv, which contains the arguments passed
to the function.

Here's another simple example:
    int args_test = 0;
    TValue func_test(VM, BijouBlock* blk, int argc, TValue* argv )
    {
        UNUSED(vm);
        UNUSED(blk);
        UNUSED(argc);
        UNUSED(argv);    
        printf("Ohai! I works!\n");
        return create_bijou_Number(12);
    }

Each function must return a TValue struct; but if you need a C type, such as a
different structure, you can use `create_pointer(void *p)` which will create a 
TValue with type BIJOU_TPOINTER whose pointer value (tvalue.value.pointer) will 
be p cast to a void *.

You can then access the pointer value of the TValue in argv, cast it to the
appropriate type, and use it as normal.

lib/net.c has some good examples of this.
