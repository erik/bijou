/* Copyright 2010 Erik Price */

#ifndef _LIB_H_
#define _LIB_H_

struct BijouVM;
struct BijouBlock;

void setup_internal_functions(struct BijouVM*);

#define FUNCTION(name) struct TValue func_##name(struct BijouVM*, struct BijouBlock*, int, struct TValue*)

FUNCTION(print);
FUNCTION(println);

/* math */
FUNCTION(sin);
FUNCTION(cos);
FUNCTION(tan);

FUNCTION(exp);
FUNCTION(log);

FUNCTION(ceil);
FUNCTION(floor);

FUNCTION(rand);
FUNCTION(rand_int);

#endif /* _LIB_H_ */
