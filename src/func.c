/* Copyright 2010 Erik Price */

#include "bijou.h"
#include "func.h"
#include "vm.h"
#include "internal.h"

BijouFunction* BijouFunction_new(BijouFunc* func, int arity, char *name, int internal, BijouBlock* b)
{
    BijouFunction *bf = B_MALLOC(sizeof(BijouFunction));
    bf->func = func;
    bf->arity = arity;
    bf->name = name;
    bf->internal = internal;
    if (! internal) {
        b->argc = arity;
        kv_init(b->locals);
    }
    bf->block = b;
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
    if (func.internal)
        return (*func.func)(vm, b, argc, args);
    else {
        BijouFrame* frame = B_ALLOC(BijouFrame);
        TValue result = bijou_interpret(vm, frame, func.block, 0, argc, args);
        B_FREE(frame);
        return result;
    }
}
