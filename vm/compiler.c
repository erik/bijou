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
#include "bopcodes.h"
#include "dump.h"
#include "bijouc.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

static int _debug_ = 0;

#define PRINTDBG(x) if(_debug_) {printf("%s",x);}
#define PRINTFDBG(x, ...) if(_debug_) {printf(x, __VA_ARGS__);}


static int writer(VM, const void *p, size_t s, void *d)
{
    UNUSED(vm);
    return (fwrite(p, s, 1, (FILE*)d) != 1) && (s != 0);
}

int compile_file(FILE* in, FILE* out, unsigned int options)
{
    if (options & OPT_DEBUG) {
        _debug_ = 1;
    }

    BijouVM* vm = BijouVM_new(0);
    BijouBlock* block = BijouBlock_new(0);

    /* TODO: Need a compile_function function! */
    compile_header(in, vm, block);
    compile_code(in, vm, block);
    compile_const(in, vm, block);

    Proto *p = to_proto(vm, block);

    if (! options & OPT_PARSE)
        bijou_dump(vm, p, writer, out);

    BijouBlock_destroy(block);
    BijouVM_destroy(vm);
    Proto_destroy(p);

    return 0;
}

#define IS(str, oth) (strcmp((str), (oth)) == 0)

void compile_header(FILE* file, VM, BijouBlock* b)
{
    PRINTDBG("Header\n");

    expect(file, ">HEAD");

    char *string = read_next(file);

    char *tmp;

    int n;

    while (!IS(string, "<HEAD")) {

        if (IS(string, ".regs")) {
            tmp = read_next(file);
            n = atoi(tmp);
            B_FREE(tmp);
            b->regc = n;
            PRINTFDBG("\tRegisters: %d\n", n);
        } else if (IS(string, ".globals")) {
            tmp = read_next(file);
            n = atoi(tmp);
            B_FREE(tmp);

            vm->numglobals = n;
            vm->globals = B_MALLOC(vm->numglobals * sizeof(TValue));

            b->numglobal = n;

            PRINTFDBG("\tGlobals: %d\n", n);
        } else if (IS(string, ".upvals")) {
            tmp = read_next(file);
            n = atoi(tmp);
            B_FREE(tmp);

            kv_init(b->upvals);

            PRINTFDBG("\tUpvals: %d\n", n);
        } else if (IS(string, ".params")) {

            tmp = read_next(file);
            n = atoi(tmp);
            B_FREE(tmp);
            b->argc = n;

            PRINTFDBG("\tParams: %d\n", n);
        } else {
            fprintf(stderr, "Don't know \"%s\"\n", string);
            exit(1);
        }
        B_FREE(string);
        string = read_next(file);
    }

    B_FREE(string);

}

static const char * opcode_names[] = { OPCODE_LABELS };
static const int  opcode_args[] = { OPCODE_ARGS };

void compile_code(FILE* file, VM, BijouBlock* b)
{
    PRINTDBG("Code\n");

    UNUSED(vm);

    expect(file, ">CODE");

    char * string = read_next(file);
    int * args;

    int index;
    int i;

    unsigned int typeargs;

    while (!IS(string, "<CODE")) {
        index = -1;
        typeargs = 0;

        for (i = 0; i < NUM_OPS; ++i) {
            if (IS(string, opcode_names[i])) {
                index = i;
                break;
            }
        }

        if (index == -1) {
            fprintf(stderr, "Don't know opcode \"%s\"\n", string);
            exit(1);
        } else {
            bInst inst;

            PRINTFDBG("\t%s\t", opcode_names[index]);
            args = read_args(file);
            PRINTDBG("\n");

            typeargs = opcode_args[index];

            SET_OPCODE(inst, index);

            if (HASARG_Bx(typeargs)) {
                SETARG_A(inst, args[0]);
                SETARG_Bx(inst, args[1]);
            }

            else if (HASARG_sBx(typeargs)) {
                int arg_a = HASARG_A(typeargs) ? args[0] : 0;
                int arg_sbx = HASARG_A(typeargs) ? args[1] : args[0];
                SETARG_A(inst, arg_a);
                SETARG_sBx(inst, arg_sbx);
            } else {
                SETARG_A(inst, args[0]);
                SETARG_B(inst, args[1]);
                SETARG_C(inst, args[2]);
            }
            BijouBlock_push_instruction(b, inst);

        }

        B_FREE(string);
        B_FREE(args);

        string = read_next(file);
    }
    B_FREE(string);
}

void compile_const(FILE* file, VM, BijouBlock* b)
{
    PRINTDBG("Const\n");

    UNUSED(vm);

    expect(file, ">CONST");

    char* line;

    int type;

    TValue t;

    line = read_line(file);

    while (!IS(line, "<CONST")) {
        PRINTFDBG("\t%s\n", line);

        type = atoi(&line[0]);

        if (!(line[1] == '#')) {
            fprintf(stderr, "Error: malformed constant: %s\n", line);
            exit(1);
        }

        t.tt = type;

        switch (type) {
        case BIJOU_TNULL:
            setnullvalue(&t);
            break;
        case BIJOU_TNUMBER:
            setnumvalue(&t, atol(line + 2));
            break;
        case BIJOU_TBOOLEAN:
            setboolvalue(&t, atoi(line + 2));
            break;
        case BIJOU_TSTRING: {
            strncpy(line, line + 3, strlen(line) - 2);
            line[strlen(line) - 1] = '\0';

            BijouString str = BijouString_new(line);
            t = create_TValue_string(str);

            B_FREE(str.ptr);
            break;
        }

        default:
            fprintf(stderr, "Unknown type: %d (%s)\n", type, line);
            exit(1);
        }

        BijouBlock_push_const(b, t);


        B_FREE(line);
        line = read_line(file);
    }
    B_FREE(line);

}

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

char* read_next(FILE* file)
{
    char *string = B_MALLOC(sizeof(char) * 1);
    size_t i;

    char c = read_char(file, 1);

    string[0] = c;

    /* TODO: may error if a comment follows something:
     * 	loadk; load
     */
    for (i = 1; !isspace(c = fgetc(file)); ++i) {

        if (c == EOF) {
            fprintf(stderr, "Unexpected EOF!\n");
            exit(1);
        }


        string = B_REALLOC(string, i + 1);
        string[i] = c;

        /* read string */
        if (c == '"') {
            /* TODO: escape sequences */
            while ((c = fgetc(file)) != '"') {
                i++;
                string = B_REALLOC(string, i + 1);
                string[i] = c;
            }

            string = B_REALLOC(string, ++i + 1);
            string[i + 1] = '"';
        }

    }

    string[i] = '\0';
    return string;
}

char *read_line(FILE* file)
{
    char* string = B_MALLOC(1 * sizeof(char));

    size_t i;
    char c;

    /* skip leading whitespace */
    while (isspace((c = fgetc(file)))) {};

    string[0] = c;

    for (i = 1; (c = fgetc(file)) != '\n'; ++i) {

        if ( c == ';') {
            while (fgetc(file) != '\n') {} ;
            return string;
        }

        string = B_REALLOC(string, i + 1);
        string[i] = c;

        /* read string */
        if (c == '"') {
            /* TODO: escape sequences */
            while ((c = fgetc(file)) != '"') {
                i++;
                string = B_REALLOC(string, i + 1);
                string[i] = c;
            }

            string = B_REALLOC(string, i++ + 2);
            string[i] = '"';
        }
    }

    string[i] = '\0';

    return string;

}

int *read_args(FILE* file)
{
    size_t index;
    int *args = B_MALLOC(3 * sizeof(int));

    for (index = 0; index < 3; index++) {
        args[index] = -1;
    }

    char *arg = read_next(file);

    index = 0;
    while (index < 3 ) {
        int const_flag = 0;
        size_t i;

        for (i = 0; i < strlen(arg); ++i) {
            if (!isdigit(arg[i])) {
                if ((i != 0 && arg[i] == '-') && arg[i] != 'K') {
                    fprintf(stderr, "Malformed argument: %s\n", arg);
                    exit(1);
                }
                if (arg[i] == 'K') const_flag = 1;
                break;
            }
        }

        PRINTFDBG("%s ", arg);

        /* if constant */
        if (const_flag) {
            args[index] = MAKEK(atoi(arg));
        } else {
            args[index] = atoi(arg);
        }

        B_FREE(arg);

        arg = B_MALLOC(1 * sizeof(char));
        char c;

        while (isspace(c = fgetc(file)) && c != '\n') {};
        if (c == '\n') {
            B_FREE(arg);
            return(args);
        }
        ungetc(c, file);

        for (i = 0; !isspace((c = fgetc(file))); ++i) {
            if (c == ';') {
                while (fgetc(file) != '\n') {};
                B_FREE(arg);
                return args;
            }

            arg = B_REALLOC(arg, i + 1);
            arg[i] = c;
        }

        arg[i] = '\0';

        index++;
    }

    B_FREE(arg);

    return args;
}

void expect(FILE* file, char* str)
{
    if (!strlen(str))
        return;

    char *s = B_MALLOC(sizeof(char) * (strlen(str) + 1));
    size_t i;

    s[0] = read_char(file, 1);

    for (i = 1; i < strlen(str); ++i) {
        s[i] = fgetc(file);
    }

    s[i] = '\0';
    if (strcmp(s, str)) {
        fprintf(stderr, "Expected \"%s\", but got \"%s\"\n", str, s);
        exit(1);
    }

    B_FREE(s);
}
