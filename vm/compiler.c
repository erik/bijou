/* (bad) Assembly format */

/*
 * Comments are indicated by a ';' and run until the end of the line
 *
 * There are 3 sections for each function:
 * 	Header:
 *	  * Contains settings for function (registers, upvals, etc)
 *      Code:
 *        * Contains opcodes and arguments
 *      Constants:
 *        * Contains the function's constants (and nested functions)
 */

/*
 * Header:
 * 	Begins with '>HEAD', and ends with '<HEAD'
 *
 *	Parameters:
 *	    .regs    [number]   Number of registers needed for function
 *	    .globals [number]   Number of globals (only in top level function)
 *          .upvals  [number]   Number of upvals for function
 *          .params  [number]   Number of parameters for function
 */

/*
 * Code:
 *	Begins with '>CODE', and ends with '<CODE'
 *
 *	On each line, there can be only one opcode and its arguments
 *
 *      For opcodes which have RK (register OR constant) arguments,
 *      follow the number with 'K':
 * 	    add 1 0 0K  ; R[1] = R[0] + K[0]
 */

/*
 * Constants:
 *	Begins with '>CONST', and ends with '<CONST'
 *
 *	Each line contains only one constant, with nested functions
 *	obviously spreading over multiple lines
 *
 *	The format is 'TYPE#CONSTANT', where type is one of the following:
 *      	BIJOU_TNULL = 0,
 *		BIJOU_TNUMBER = 1,
 *	        BIJOU_TBOOLEAN = 2,
 *		BIJOU_TSTRING = 3,
 //		TODO: BIJOU_TFUNCTION = 4 //
 *
 *	  1#42 ; constant of type 1 (number), with the value 42
 */


#include "compiler.h"
#include "bijou.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


char read_char(FILE* file, int ignore_ws)
{
    char c;

redo:
    c = fgetc(file);
    if (ignore_ws && isspace(c))
        goto redo;

    /* comment */
    else if (c == ';') {
        /* skip until EOL */
        while ( (c = fgetc(file)) != '\n') {};
        goto redo;
    } else
        return c;
}

int compile_file(FILE* in, FILE* out, unsigned int options)
{
    UNUSED(out);
    UNUSED(options);

    BijouVM* vm = BijouVM_new(0);
    BijouBlock* block = BijouBlock_new(0);

    compile_header(in, vm, block);

    return 0;
}

void compile_header(FILE* file, VM, BijouBlock* b)
{
    UNUSED(file);
    UNUSED(vm);
    UNUSED(b);
    
    expect(file, ">HEAD");

}


void expect(FILE* file, char* str)
{
    if (!strlen(str))
	return;
    
    char *s = B_MALLOC(sizeof(char) * (strlen(str) + 1));
    size_t i;

    s[0] = read_char(file, 1);
    
    for(i = 1; i < strlen(str); ++i) {
	s[i] = fgetc(file);
    }

    if (strcmp(s, str)) {
	fprintf(stderr, "Expected \"%s\", but got \"%s\"\n", str, s);
	exit(1);
    }

    B_FREE(s);
}
