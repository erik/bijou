/* Copyright 2010 Erik Price */

#include "bijou.h"
#include "func.h"

BijouFunction* BijouFunction_new(BijouFunc* func, int arity, char *name)
{
    BijouFunction *bf = B_MALLOC(sizeof(BijouFunction));
    bf->func = func;
    bf->arity = arity;
    bf->name = name;
    return bf;
}

void BijouFunction_destroy(BijouFunction* func)
{
    B_FREE(func);
}

TValue BijouFunction_call(VM, BijouBlock* b, BijouFunction func, TValue* args, int argc)
{
    if ((func.arity != -1) && (argc != func.arity)) {
        fprintf(stderr, "Wrong number of arguments to %s, (%d for %d)\n", func.name, argc, func.arity);
        exit(0);
    }
    return (*func.func)(vm, b, argc, args);
}
