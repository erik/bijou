#include "bijou.h"
#include "load.h"
#include "vm.h"
#include "internal.h"
#include "compiler.h"

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
    size_t s = (*S->reader)(S, b, size);
    UNLESS(s, "Unexpected end of file");
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
    }
}


static void LoadHeader(LoadState* S)
{
    char h[BIJOU_HEADERSIZE];
    char s[BIJOU_HEADERSIZE];

    make_header(h);

    LoadBlock(S, s, BIJOU_HEADERSIZE);
}

Proto* bijou_load(VM, BijouBlock* b, bijou_Reader* reader, const char *name)
{
    LoadState S;
    S.name = name;
    S.vm = vm;
    S.block = b;
    S.reader = reader;

    LoadHeader(&S);
    return;
}
