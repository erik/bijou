/* Copyright 2010 Erik Price */

#include "bijou.h"
#include "load.h"
#include "vm.h"
#include "internal.h"
#include "dump.h"

#include <string.h>

#define IF(c, s)     if (c) error(S, s);
#define UNLESS(c, s) IF(!(c), s)

static void error(LoadState* S, const char * msg)
{
    fprintf(stderr, "%s: %s in bytecode\n", S->name, msg);
    exit(EXIT_FAILURE);
}

#define LoadMem(S, b, n, size)     LoadBlock(S, b, (n) * (size))
#define LoadByte(S)                (unsigned char)LoadChar(S)
#define LoadVar(S, x)              LoadMem(S, &x, 1, sizeof(x))
#define LoadVector(S, b, n, size)  LoadMem(S, b, n, size)

static void LoadBlock(LoadState* S, void* b, size_t size)
{

    int s = (*S->reader)(S, b, size);
    IF(s == EOF, "Unexpected end of file");
}

static int LoadChar(LoadState* S)
{
    char x;
    LoadVar(S, x);
    return x;
}

static int LoadInt(LoadState* S)
{
    int x;
    LoadVar(S, x);
    IF (x < 0, "bad integer");
    return x;
}

static bijou_Number LoadNumber(LoadState* S)
{
    bijou_Number x;
    LoadVar(S, x);
    return x;
}


static char* LoadString(LoadState* S)
{
    size_t size;
    LoadVar(S, size);
    if (size == 0)
        return NULL;
    else {
        char *s = B_MALLOC(sizeof(char) * size);
        LoadBlock(S, s, size);
        return s;
    }
}

static void LoadCode(LoadState* S, Proto* f)
{
    int n = LoadInt(S);
    f->code = B_MALLOC(sizeof(bInst) * n);
    f->sizecode = n;

    LoadVector(S, f->code, n, sizeof(bInst));

}
static Proto* LoadFunction(LoadState *S);

static void LoadConstants(LoadState* S, Proto* f)
{
    int i, n;
    n = LoadInt(S);
    f->k = B_MALLOC(sizeof(TValue) * n);
    f->sizek = n;

    for (i = 0; i < n; ++i) {
        setnullvalue(&f->k[i]);
    }
    for (i = 0; i < n; ++i) {
        TValue* obj = &f->k[i];
        int type = LoadChar(S);

        switch (type) {
        case BIJOU_TNULL:
            setnullvalue(obj);
            break;
        case BIJOU_TNUMBER: {
            bijou_Number n = LoadNumber(S);
            setnumvalue(obj, n);
            break;
        }
        case BIJOU_TBOOLEAN:
            setboolvalue(obj, LoadChar(S) != 0);
            break;
        case BIJOU_TSTRING: {
            char * s = LoadString(S);
            BijouString bstr = BijouString_new(s);
            *obj = create_TValue_string(bstr);

            break;
        }
        /* handled below, so nothing to do here */
        case BIJOU_TFUNCTION:
            break;
        default:
            fprintf(stderr, "Got: %d\n", type);
            error(S, "Unknown type");
            break;
        }
    }

    n  = LoadInt(S);
    f->p = B_MALLOC(sizeof(Proto*) * n);
    f->sizep = n;

    for (i = 0; i < n; ++i) {
        f->p[i] = NULL;
    }

    for (i = 0; i < n; ++i) {
        f->p[i] = LoadFunction(S);
    }
}

static Proto* LoadFunction(LoadState* S)
{
    Proto* f = B_ALLOC(Proto);

    char *str = LoadString(S);
    f->source = BijouString_new(str);

    str = LoadString(S);
    f->name = BijouString_new(str);

    f->linedefined = LoadInt(S);
    f->lastlinedefined = LoadInt(S);

    f->maxregisters = (u_byte)LoadChar(S);
    f->numglobal = (u_byte)LoadChar(S);

    f->numupval = (u_byte)LoadChar(S);
    f->numparam = (u_byte)LoadChar(S);
    LoadCode(S, f);
    LoadConstants(S, f);

    return f;
}


static void LoadHeader(LoadState* S)
{
    char h[BIJOU_HEADERSIZE];
    char s[BIJOU_HEADERSIZE];

    make_header(h);

    LoadBlock(S, s, BIJOU_HEADERSIZE);
    IF(memcmp(h, s, BIJOU_HEADERSIZE), "bad header, recompile program");
}

Proto* bijou_load(VM, BijouBlock* b, bijou_Reader reader, const char *name, FILE* fp)
{
    LoadState S;
    S.name = name;
    S.vm = vm;
    S.block = b;
    S.reader = reader;
    S.fp = fp;

    LoadHeader(&S);
    return LoadFunction(&S);
}


/* doesn't mess with globals */
static BijouBlock* proto_to_block2(VM, Proto* p, int setglobals)
{
    size_t i;

    BijouBlock* block = BijouBlock_new(0);

    block->filename = p->source.ptr;
    block->funcname = p->name.ptr;

    if (setglobals) {
        vm->numglobals = p->numglobal;
        vm->globals = B_MALLOC(sizeof(TValue) * vm->numglobals);
    }

    block->filename = BijouString_to_cstring(p->source);

    block->linedefined = p->linedefined;
    block->lastlinedefined = p->linedefined;

    block->regc = p->maxregisters;

    block->argc = p->numparam;

    for (i = 0; i < p->sizek; ++i) {
        BijouBlock_push_const(block, p->k[i]);
    }

    for ( i = 0; i < p->sizecode; ++i) {
        BijouBlock_push_instruction(block, p->code[i]);
    }

    block->numchildren = p->sizep;
    block->children = B_MALLOC(p->sizep * sizeof(BijouBlock));

    for (i = 0; i < p->sizep; ++i) {
        block->children[i] = proto_to_block2(vm, p->p[i], 0);
    }

    return block;
}


BijouBlock* proto_to_block(VM, Proto* p)
{
    return proto_to_block2(vm, p, 1);
}
