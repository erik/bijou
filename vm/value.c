/* TValue functions */

#include "bijou.h"
#include "internal.h"
#include <string.h>
#include <stdio.h>


/* TValue to bijou_Number */
#define TV2BN(o)        (o).value.n

/* create a TValue from a number */
TValue create_bijou_Number(bijou_Number num)
{
        Value v;
        v.n = num;
        TValue ret;
        ret.tt = BIJOU_TNUMBER;
        ret.value = v;
        return ret;
}

/* returns nonzero if the two parameters are equal,
 * zero otherwise.
 */
int TValue_equal(TValue f, TValue s)
{
        /* lol, ternary */
        return f.tt == s.tt ? f.tt == BIJOU_TNULL ? 1 : \
               f.tt == BIJOU_TNUMBER || f.tt == BIJOU_TBOOLEAN \
               ? (f.value.n == s.value.n) : f.tt == BIJOU_TSTRING ? \
               strcmp(f.value.s.ptr, s.value.s.ptr) == 0 : &f == &s : 0;
}


/* returns a string representation of t */
char *TValue_to_string(TValue t)
{
        int type = t.tt;
        switch (type) {
        case BIJOU_TNONE:
                return "NONE";
        case BIJOU_TNULL:
                return "NULL";
        case BIJOU_TNUMBER: {
                char * ca = B_MALLOC(sizeof(char) * 20);
                memset(ca, '\0', 20);
                sprintf(ca, "%f", (double) t.value.n );
                return ca;
        }
        case BIJOU_TSTRING:
                return t.value.s.ptr;
        case BIJOU_TBOOLEAN:
                return t.value.b ? "true" : "false";
        default:
                return "Nope, not yet";
        }
}

/* return a BijouString representation of t */
BijouString TValue_to_BijouString(TValue t)
{
        return BijouString_new(TValue_to_string(t));
}
