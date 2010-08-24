/* Copyright 2010 Erik Price */

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
static size_t _indent_ = 0;

#define INDENT { size_t __i; for(__i = 0; __i < _indent_; ++__i) printf("\t"); }
#define PRINTDBG(x) if(_debug_) { INDENT; printf("%s", x); }
#define PRINTFDBG(x, ...) if(_debug_) { INDENT; printf(x, __VA_ARGS__); }
#define NOIND_PRINTFDBG(x, ...) if(_debug_) { printf(x, __VA_ARGS__); }


static int writer(VM, const void *p, size_t s, void *d)
{
    UNUSED(vm);
    return (fwrite(p, s, 1, (FILE*)d) != 1) && (s != 0);
}

int compile_file(FILE* in, FILE* out, char* filename, unsigned int options)
{
    if (options & OPT_DEBUG) {
        _debug_ = 1;
    }

    BijouVM* vm = BijouVM_new(0);
    BijouBlock* block = BijouBlock_new(0);

    block->filename = filename;
    block->funcname = "main";
    compile_function(in, vm, block);

    Proto *p = to_proto(vm, block);

    if (! options & OPT_PARSE)
        bijou_dump(vm, p, writer, out);

    BijouBlock_destroy(block);
    BijouVM_destroy(vm);
    Proto_destroy(p);

    return 0;
}

void compile_function(FILE* file, VM, BijouBlock*b)
{
    compile_header(file, vm, b);
    compile_code(file, vm, b);
    compile_const(file, vm, b);
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
        }  else if (IS(string, ".name")) {
            b->funcname = read_next(file);

            PRINTFDBG("\tName: %s\n", b->funcname);
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
            bInst inst = 0;

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

        type = atoi(&line[0]);
        if ( line[0] == ';') {
            B_FREE(line);
            line = read_line(file);
            continue;
        }
        if (!(line[1] == '#')) {
            fprintf(stderr, "Error: malformed constant: %s\n", line);
            exit(1);
        }

        t.tt = type;

        if (type != BIJOU_TFUNCTION) {
            PRINTFDBG("\t%s\n", line);
        }

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
            BijouString str = BijouString_new(line + 3);
            t = create_TValue_string(str);

            break;
        }
        case BIJOU_TFUNCTION: {
            _indent_++;
            BijouBlock *func = BijouBlock_new(b);
            compile_function(file, vm, func);
            BijouBlock_push_child(b, func);
            break;
        }
        case BIJOU_TPOINTER: {
            fprintf(stderr, "Error: trying to save a pointer object\n");
            exit(1);
        }
        default:
            fprintf(stderr, "Unknown type: %d (%s)\n", type, line);
            exit(1);
        }

        BijouBlock_push_const(b, t);


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

            string = B_REALLOC(string, i + 1);
            string[i + 1] = '"';
        }

    }

    string[i] = '\0';
    return string;
}

char *read_line(FILE* file)
{
    char* string = B_MALLOC(1 * sizeof(char));
    memset(string, '\0', 1);

    size_t i;
    char c;

    /* skip leading whitespace */
    while (isspace((c = fgetc(file)))) {};

    string[0] = c;

    for (i = 1; (c = fgetc(file)) != '\n'; ++i) {

        if ( c == ';') {
            string[i - 1] = '\0';
            while (fgetc(file) != '\n') {} ;
            return string;
        }

        string = B_REALLOC(string, i + 1);
        string[i] = c;

        /* read string */
        if (c == '"') {
            while ((c = fgetc(file)) != '"') {
                if (c == EOF) {
                    fprintf(stderr, "Unexpected EOF while reading string: %s\n",
                            string);
                    exit(1);
                } else if (c == '\n') {
                    fprintf(stderr, "Unterminated string: %s\n", string);
                    exit(1);
                }
                i++;
                string = B_REALLOC(string, i + 1);
                string[i] = c;
            }

            string = B_REALLOC(string, i++);
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

        NOIND_PRINTFDBG("%s ", arg);

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
