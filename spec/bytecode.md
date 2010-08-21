#Bytecode Specification
*Copyright &copy; 2010, Erik Price*

##String format:
       4 bytes   : string size (including trailing \0)
       (variable): bytes (including trailing \0)


##List format:
   Constant list:

     4 bytes : size of constant list (number of entries)
     [
       1 byte: the type of the constant
               (BIJOU_TNULL = 0, BIJOU_TNUMBER = 1,
                BIJOU_TBOOLEAN = 2, BIJOU_TSTRING = 3)
       const : the actual constant. size depends on type.
               --BIJOU_TNULL will have a size of 0 (nothing)
               --BIJOU_TNUMBER will have 8 bytes, the number
               --BIJOU_TBOOLEAN will have 1 byte, 0 for false, 1 for true
               --BIJOU_TSTRING will use the format described above
     ]

   Instruction list:

     4 bytes : size of code (number of instructions)
     [
       Instruction
     ]

   Function prototype list:

     4 bytes : number of function prototypes
     [
       String  : name of source file ( length of 0 if not top level function)
       4 bytes : line defined
       4 bytes : last line defined
       1 byte  : maximum number of registers needed (for current function)
       1 byte  : number of globals (ignored if not top level function)
       1 byte  : number of upvals
       1 byte  : number of parameters
       List    : list of instructions (code)
       List    : list of constants (includes function prototpes) (for current function)
     ]


##Bytecode format:
   Header (6 bytes)

     4 bytes : signature (10 62 69 6A) "\16bij"
     1 byte  : version number (example: 0x16, major = 1, minor = 6)
     1 byte  : endianness

   Top level function (main)

     String  : name of source file
     4 bytes : line defined
     4 bytes : last line defined
     1 byte  : maximum number of registers needed (for current function)
     1 byte  : number of globals (ignored if not top level function)
     1 byte  : number of upvals
     1 byte  : number of parameters
     List    : list of instructions (code)
     List    : list of constants (for current function)

