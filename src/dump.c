/* Copyright 2010 Erik Price */

#include "config.h"
#include "bijou.h"
#include "internal.h"
#include "dump.h"
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
        char* str = s->ptr;

        char* pch = strchr(str, '\\');

        while (pch != NULL) {
            size_t i = pch - str;
            char c = pch[1];

            if ( i + 1 >= --size) {
                fprintf(stderr, "Unterminated string: %s\n", str);
                exit(1);
            }


            switch (c) {
            case 'n':
                c = '\n';
                break;
            case 'r':
                c = '\r';
                break;
            case 't':
                c = '\t';
                break;
            case 'a':
                c = '\a';
                break;
            case '\\':
                c = '\\';
                break;
            default: {
                fprintf(stderr, "Unrecognized escape \"\\%c\" in string %s\n", c, str);
                exit(1);
            }
            }

            pch[1] = c;

            strcpy(str + i, pch + 1);

            pch = strchr(pch +  1, '\\');

        }
        DumpVar(size, D);
        DumpBlock(str, size, D);
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

            /* handled below, so no need to do anything here */
        case BIJOU_TFUNCTION:
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
    DumpString(&p->name, D);

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
    p->name = BijouString_new(b->funcname);

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

    p->sizep = b->numchildren;
    p->p = B_MALLOC(sizeof(BijouBlock*) * p->sizep);

    for (i = 0; i < b->numchildren; ++i) {
        p->p[i] = to_proto(vm, b->children[i]);
    }

    return p;

}

void Proto_destroy(Proto *p)
{

    size_t i;
    for (i = 0; i < p->sizep; ++i) {
        B_FREE(p->p[i]);
    }
    B_FREE(p->p);
    B_FREE(p->code);
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
