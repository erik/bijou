/* Copyright 2010 Erik Price */

#ifndef _LIB_H_
#define _LIB_H_

struct BijouVM;
struct BijouBlock;

void setup_internal_functions(struct BijouVM*);

struct TValue func_print(struct BijouVM*, struct BijouBlock*, int, struct TValue*);
struct TValue func_println(struct BijouVM*, struct BijouBlock*, int, struct TValue*);

struct TValue func_sin(struct BijouVM*, struct BijouBlock*, int, struct TValue*);
struct TValue func_cos(struct BijouVM*, struct BijouBlock*, int, struct TValue*);
struct TValue func_tan(struct BijouVM*, struct BijouBlock*, int, struct TValue*);

#endif /* _LIB_H_ */
