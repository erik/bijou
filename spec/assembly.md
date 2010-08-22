#(bad) Assembly format 
*Copyright &copy; 2010, Erik Price*
 
There are 3 sections for each function:

1. Header:
     - Contains settings for function (registers, upvals, etc)
2. Code:
      - Contains opcodes and arguments
3. Constants:
      - Contains the function's constants (and nested functions)

Comments are indicated by a ';' and run until the end of the line

##Header:
 * Begins with '>HEAD', and ends with `<HEAD`
 * Parameters:
   1.  `.regs    [number]` Number of registers needed for function
   2.  `.globals [number]` Number of globals (only in top level function)
   3.  `.upvals  [number]` Number of upvals for function
   4.  `.params  [number]` Number of parameters for function
   5.  `.name    [name]`   Name of function (not a string -- `.name my_func`)


##Code:
 * Begins with '>CODE', and ends with `<CODE`
 * On each line, there can be only one opcode and its arguments
 
   For opcodes which have RK (register OR constant) arguments,
   follow the number with 'K':
   `add 1 0 0K  ; R[1] = R[0] + K[0]`

   The 'closure' opcode is special in that if the Bx argument is followed
   by a K, then it will use the corrosponding internal function, otherwise
   use the bytecode function at that index.


##Constants:
 * Begins with '>CONST', and ends with `<CONST`

 * Each line contains only one constant, with nested functions
  obviously spreading over multiple lines

 *	The format is 'TYPE#CONSTANT', where type is one of the following:
          BIJOU_TNULL = 0,
          BIJOU_TNUMBER = 1,
          BIJOU_TBOOLEAN = 2,
          BIJOU_TSTRING = 3,
          BIJOU_TFUNCTION = 4 
 
 `1#42 ; constant of type 1 (number), with the value 42`

