/* Copyright 2010 Erik Price */

#ifndef _FUNC_H_
#define _FUNC_H_

struct BijouFunction;

BijouFunction* BijouFunction_new(BijouFunc* func, int arity, char* name);
void BijouFunction_destroy(BijouFunction*);
TValue BijouFunction_call(VM, BijouBlock*, BijouFunction func, TValue* args, int argc);

#endif /* _FUNC_H_ */
