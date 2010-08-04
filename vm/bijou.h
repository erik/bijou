/*
 * Various utility definitions and functions for the Bijou virtual Machine
 */

#ifndef _BIJOU_H_
#define _BIJOU_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>

#include "vendor/kvec.h"

#include "config.h"

#define BIJOU_SIGNATURE "\020bij"

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


struct BijouVM;
struct BijouFrame;

typedef struct BijouString {
        char *ptr;
        size_t len;
} BijouString;


typedef union {
        BijouString s;        /* string */
        bijou_Number n; /* number */
        int b;          /* boolean */
} Value;

/* tagged value */
typedef struct bijou_TValue {
        Value value;
        int tt; /* tagged type */
} TValue;

/* pointer to stack index */
typedef TValue* StkId;

#define ttype(o)        ((o)->tt)

/* macros to test type of TValue */
#define ttisnull(o)     (ttype(o) == BIJOU_TNULL)
#define ttisnumber(o)   (ttype(o) == BIJOU_TNUMBER)
#define ttisboolean(o)  (ttype(o) == BIJOU_TBOOLEAN)
#define ttisstring(o)   (ttype(o) == BIJOU_TSTRING)
#define ttisfunction(o) (ttype(o) == BIJOU_FUNCTION)

#define isfalse(o)      (ttisnull(o) || (ttisboolean(o) && bvalue(o) == 0))

#define bvalue(o)       check_exp(ttisboolean(o), o->value.b)

/* macros to set values */
#define setnullvalue(o)     ((o))->tt  = BIJOU_TNULL)
#define setnumvalue(o, x)   { ((o)->tt = BIJOU_TNUMBER); (o)->value.n=(x); }
#define setboolvalue(o, x)  { ((o)->tt = BIJOU_TBOOLEAN); (o)-value.b=(x); }

#define settype(o, t)       { (o)->tt  = t; }

/* TValue to bijou_Number */
#define TV2BN(o)        (o).value.n

typedef struct BijouBlock {
        kvec_t(TValue) k;               /* constants */
        kvec_t(BijouString) strings;    /* string pool */
        kvec_t(TValue) locals;          /* local variables */
        kvec_t(TValue) upvals;          /* upvalues */
        kvec_t(bInst) code;             /* actual code */
        size_t regc;                    /* register count */
} BijouBlock;

typedef struct BijouFrame {
        TValue *stack;
        TValue *upvals;
} BijouFrame;


/* TODO: write me! */
typedef struct BijouVM {
        int temp;
} BijouVM;

/* block prototypes */
BijouBlock *BijouBlock_new(void);
int BijouBlock_push_const(BijouBlock*, TValue);
int BijouBlock_push_local(BijouBlock*, TValue);
int BijouBlock_find_const(BijouBlock*, TValue);
int BijouBlock_find_local(BijouBlock*, TValue);
int BijouBlock_push_string(BijouBlock*, BijouString);
int BijouBlock_find_string(BijouBlock*, BijouString);
int BijouBlock_push_instruction(BijouBlock*, bInst);
bInst BijouBlock_fetch_instruction(BijouBlock*, int);
void BijouBlock_dump(BijouBlock*);

/* TValue manipulation functions */
TValue create_bijou_Number(bijou_Number);
TValue create_boolean(bijou_Number);
TValue create_null(void);
TValue create_TValue_string(BijouString);
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

#endif /* _BIJOU_H_ */
