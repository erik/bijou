/*
 * Various utility definitions and functions for the Bijou virtual Machine
 */
/* Copyright 2010 Erik Price */

#ifndef _BIJOU_H_
#define _BIJOU_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <math.h>

#include "vendor/kvec.h"

#include "config.h"

#define BIJOU_SIGNATURE "\020bij"
#define BIJOU_VERSION   0x01

#define UNUSED(x)     ((void)(x))

#ifdef GC
#include "gc.h"
#define B_MALLOC      GC_MALLOC
#define B_CALLOC(m,n) GC_MALLOC((m)*(n))
#define B_REALLOC     GC_REALLOC
#define B_FREE(x)     UNUSED(x)
#else
#define B_MALLOC      malloc
#define B_CALLOC(m,n) malloc((m)*(n))
#define B_REALLOC     realloc
#define B_FREE(x)     free(x)
#endif

#define VM            struct BijouVM *vm

/* use 32 bit ints as instructions */
typedef unsigned int bInst;


/* some basic types */

#define BIJOU_TNONE        (-1)

#define BIJOU_TNULL        (0)
#define BIJOU_TNUMBER      (1)
#define BIJOU_TBOOLEAN     (2)
#define BIJOU_TSTRING      (3)
#define BIJOU_TFUNCTION    (4)

/* TODO!
 * #define BIJOU_TARR         (5)
*/

/* type of numbers is always float (for now) */
typedef BIJOU_DOUBLE bijou_Number;

typedef struct BijouString {
    char *ptr;
    size_t len;
} BijouString;


typedef union {
    BijouString s;  /* string */
    bijou_Number n; /* number */
    struct BijouFunction* func; /* function */
    int b;          /* boolean */
} Value;

/* tagged value */
typedef struct bijou_TValue {
    Value value;
    int tt; /* tagged type */
} TValue;

struct BijouVM;
struct BijouFrame;

/* function returning a TValue */
typedef TValue (BijouFunc)(VM, int argc, TValue* argv);

typedef struct BijouFunction {
    BijouFunc* func;
    int arity;
    BijouString name;
} BijouFunction;


#define ttype(o)        ((o)->tt)

/* macros to test type of TValue */
#define ttisnull(o)     (ttype(o) == BIJOU_TNULL)
#define ttisnumber(o)   (ttype(o) == BIJOU_TNUMBER)
#define ttisboolean(o)  (ttype(o) == BIJOU_TBOOLEAN)
#define ttisstring(o)   (ttype(o) == BIJOU_TSTRING)
#define ttisfunction(o) (ttype(o) == BIJOU_TFUNCTION)

#define isfalse(o)      (ttisnull(o) || (ttisboolean(o) && bvalue(o) == 0))

#define bvalue(o)       check_exp(ttisboolean(o), o.value.b)
#define numvalue(o)     check_exp(ttisnumber(o), o.value.n)


/* macros to set values */
#define setnullvalue(o)     { (o)->tt  = BIJOU_TNULL; }
#define setnumvalue(o, x)   { ((o)->tt = BIJOU_TNUMBER); (o)->value.n=(x); }
#define setboolvalue(o, x)  { ((o)->tt = BIJOU_TBOOLEAN); (o)->value.b=(x); }

#define settype(o, t)       { (o)->tt  = t; }

/* TValue to bijou_Number */
#define TV2BN(o)        (o).value.n

typedef struct BijouBlock {
    kvec_t(TValue) k;               /* constants */
    kvec_t(TValue) locals;          /* local variables */
    kvec_t(TValue) upvals;          /* upvalues */
    kvec_t(bInst) code;             /* actual code */
    u_byte regc;                    /* register count */
    u_byte numglobal;
    u_byte argc;
    uint32_t linedefined;           /* line number */
    uint32_t lastlinedefined;
    char * filename;                /* file name */

    u_byte numchildren;
    struct BijouBlock **children;

    struct BijouBlock *parent;      /* parent block */

} BijouBlock;

typedef struct BijouFrame {
    TValue *stack;
    TValue *upvals;
} BijouFrame;

/*
 * Parameters:
 * vm - may be unused, various uses
 * p - what to write
 * s - how much to write
 * d - FILE* (or something else)
 */
typedef int (*bijou_Writer)(struct BijouVM *vm, const void *p, size_t s, void* d);

struct LoadState;
/*
 * Parameters:
 * S - LoadState (various usages, may be unused)
 * b - where to read the bytes
 * size - number of bytes to be read
 * returns: bytes read
 */
typedef int (*bijou_Reader)(struct LoadState* S, u_byte *b, size_t size);

/* block prototypes */
BijouBlock *BijouBlock_new(BijouBlock*);
void BijouBlock_destroy(BijouBlock*);
int BijouBlock_push_const(BijouBlock*, TValue);
int BijouBlock_push_local(BijouBlock*, TValue);
int BijouBlock_find_const(BijouBlock*, TValue);
int BijouBlock_find_local(BijouBlock*, TValue);
int BijouBlock_push_string(BijouBlock*, BijouString);
int BijouBlock_find_string(BijouBlock*, BijouString);
int BijouBlock_push_instruction(BijouBlock*, bInst);
bInst BijouBlock_fetch_instruction(BijouBlock*, int);
int BijouBlock_push_child(BijouBlock*, BijouBlock*);
void BijouBlock_dump(BijouBlock*);
void BijouBlock_dump2(BijouBlock*, int);

/* TValue manipulation functions */
TValue create_bijou_Number(bijou_Number);
TValue create_boolean(bijou_Number);
TValue create_null(void);
TValue create_none(void);
TValue create_TValue_string(BijouString);
const TValue *to_number(const TValue *, TValue *);
int TValue_equal(TValue, TValue);
char *TValue_to_string(TValue);
BijouString TValue_to_BijouString(TValue);
char *TValue_type_to_string(TValue);

/* String functions */
BijouString BijouString_new(char * str);
BijouString BijouString_cat(BijouString, BijouString);
int BijouString_len(BijouString);
BijouString BijouString_substr(BijouString, int, int);
int BijouString_equal(BijouString, BijouString);
char *BijouString_to_cstring(BijouString);

/* Numeric functions */
TValue TValue_num_add(TValue, TValue);
TValue TValue_num_sub(TValue, TValue);
TValue TValue_num_mul(TValue, TValue);
TValue TValue_num_div(TValue, TValue);
TValue TValue_num_pow(TValue, TValue);
TValue TValue_num_rem(TValue, TValue);
TValue TValue_num_unm(TValue);
TValue TValue_num_lt(TValue, TValue);
TValue TValue_num_le(TValue, TValue);
TValue TValue_num_gt(TValue, TValue);
TValue TValue_num_ge(TValue, TValue);

/* compiler */
//BijouCompiler *BijouCompiler_new(VM, char*);
#endif /* _BIJOU_H_ */
