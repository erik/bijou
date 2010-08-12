/* TODO: most of this stuff should be moved to a separate file, as
 * as 'compiler.c' isn't completely accurate
 */

/* Bytecode Specification */

/* String format:
   4 bytes   : string size (including trailing \0)
   (variable): bytes (including trailing \0)
*/

/* List format:
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
*/

/* bytecode format:
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

*/

#include "config.h"
#include "bijou.h"
#include "internal.h"
#include "compiler.h"
#include "bopcodes.h"

#include <string.h>

#define DumpMem(b,n,size,D)     DumpBlock(b, (n) * (size), D)
#define DumpVar(v, D)           DumpMem(&v, 1, sizeof(v), D)

static void DumpBlock(const void *blk, size_t size, DumpState *D)
{
    D->status = (*D->writer)(D->vm, blk, size, D->data);
}

static inline void DumpChar(char x, DumpState *D)
{
    DumpVar(x, D);
}

static inline void DumpInt(int x, DumpState *D)
{
    DumpVar(x, D);
}

static inline void DumpNumber(bijou_Number x, DumpState *D)
{
    DumpVar(x, D);
}

void make_header (char* h)
{
    int x = 1;
    memcpy(h, BIJOU_SIGNATURE, sizeof(BIJOU_SIGNATURE) - 1 );
    h += sizeof(BIJOU_SIGNATURE) - 1;
    *h++ = (char)BIJOU_VERSION;
    *h++ = (char) *(char*)&x;         /* endianness */
}

static void DumpHeader(DumpState *D)
{
    char h[BIJOU_HEADERSIZE];
    make_header(h);
    DumpBlock(h, BIJOU_HEADERSIZE, D);
}

static void DumpString(BijouString *s, DumpState *D)
{
    size_t size = 0;
    if ( s == NULL || s->len == 0)
        DumpVar(size, D);
    else {
        size = s->len + 1;
        DumpVar(size, D);
        DumpBlock(s->ptr, size, D);
    }
}

static inline void DumpVector(void *b, int n, size_t size, DumpState *D)
{
    DumpInt(n, D);
    DumpMem(b, n, size, D);
}
#define DumpCode(f,D)	 DumpVector(f->code,f->sizecode,sizeof(bInst),D)

static void DumpFunction(Proto *p, DumpState *D);

static void DumpConstants(Proto *p, DumpState* D)
{
    int sizek = p->sizek;
    DumpInt(sizek, D);

    int i;
    for (i = 0; i < sizek; ++i) {
        TValue o = p->k[i];

        DumpChar(ttype(&o), D);

        switch (ttype(&o)) {

        case BIJOU_TNULL:
            break;

        case BIJOU_TNUMBER:
            DumpNumber(numvalue(o), D);
            break;

        case BIJOU_TBOOLEAN:
            DumpChar(bvalue(o), D);
            break;

        case BIJOU_TSTRING:
            DumpString(&o.value.s, D);
            break;

        default:
            fprintf(stderr, "Unrecognized type!\n");
            break;
        }
    }

    int sizep = p->sizep;
    DumpInt(sizep, D);

    for (i = 0; i < sizep; ++i) {
        DumpFunction(p->p[i], D);
    }
}


static void DumpFunction(Proto *p, DumpState *D)
{
    DumpString(&p->source ?  &p->source : NULL , D);

    DumpInt(p->linedefined, D);
    DumpInt(p->lastlinedefined, D);

    DumpChar(p->maxregisters, D);
    DumpChar(p->numglobal, D);
    DumpChar(p->numupval, D);
    DumpChar(p->numparam, D);

    DumpCode(p, D);
    DumpConstants(p, D);

}

int bijou_dump(VM, Proto* proto, bijou_Writer w, void *data)
{
    DumpState D;
    D.vm = vm;
    D.writer = w;
    D.data = data;
    D.status = 0;
    DumpHeader(&D);
    DumpFunction(proto, &D);
    return D.status;
}


Proto* to_proto(VM, BijouBlock *b)
{
    UNUSED(vm);

    Proto *p = B_ALLOC(Proto);

    p->source = BijouString_new(b->filename);
    p->linedefined = b->linedefined;
    p->lastlinedefined = b->lastlinedefined;

    p->maxregisters = b->regc;
    p->numglobal = b->numglobal;

    p->numupval = kv_size(b->upvals);
    p->numparam = b->argc;

    p->sizek = kv_size(b->k);
    p->k = B_MALLOC(sizeof(TValue) * p->sizek);

    size_t i;
    for (i = 0; i < kv_size(b->k); ++i) {
        p->k[i] = kv_A(b->k, i);
    }

    p->sizecode = kv_size(b->code);
    p->code = B_MALLOC(sizeof(bInst) * p->sizecode);

    for (i = 0; i < kv_size(b->code); ++i) {
        p->code[i] = kv_A(b->code, i);
    }

    return p;

}

void Proto_destroy(Proto *p)
{
    B_FREE(p->k);
    B_FREE(p->code);
    size_t i;
    for (i = 0; i < p->sizep; ++i) {
        B_FREE(p->p[i]);
    }
    B_FREE(p);
}


BijouCompiler *BijouCompiler_new(VM, char *name)
{
    BijouCompiler *c = B_ALLOC(BijouCompiler);
    c->line = 1;
    c->vm = vm;
    c->filename = name;
    c->block = BijouBlock_new(0);
    return c;
}

/* returns a pointer to the bInst containing the code */
bInst* BijouCompiler_compile(BijouCompiler *c)
{
    /* sig + version */
    bInst *compiled_code = B_MALLOC(BIJOU_HEADERSIZE);
    BijouBlock *b = c->block;
    b->filename = c-> filename;
    PUSH_INST_A(b, RETURN, 0);
    return compiled_code;
}
