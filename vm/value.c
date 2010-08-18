/* TValue functions */

#include "bijou.h"
#include "internal.h"

#include <string.h>
#include <stdio.h>

const TValue *to_number(const TValue *obj, TValue *n)
{
    if (ttisnumber(obj))
        return obj;
    /* TODO: fix this at some point to be less ugly */
    if (ttisstring(obj)) {
        setnumvalue(n, strtod(obj->value.s.ptr, NULL));
        return n;
    } else {
        return NULL;
    }
}

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

/* create a boolean from val, 0 => false, not-0 => true */
TValue create_boolean(bijou_Number val)
{
    Value v;
    v.b = val;
    TValue ret;
    ret.tt = BIJOU_TBOOLEAN;
    ret.value = v;
    return ret;
}

/* create a TValue with the value of null */
TValue create_null(void)
{
    TValue ret;
    ret.tt = BIJOU_TNULL;
    return ret;
}

/* return a TValue with the value BIJOU_TNONE */
TValue create_none(void)
{
    TValue ret;
    ret.tt = BIJOU_TNONE;
    return ret;
}


/* create a TValue from a BijouString */
TValue create_TValue_string(BijouString s)
{
    TValue tv;
    Value v;

    v.s = s;
    tv.tt = BIJOU_TSTRING;
    tv.value = v;

    return tv;

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
           BijouString_equal(f.value.s, s.value.s) : &f == &s : 0;
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
        char *ca = B_MALLOC(sizeof(char) * 20);
        memset(ca, '\0', 20);
        sprintf(ca, "%f", (double) t.value.n );
        return ca;
    }
    case BIJOU_TSTRING:
        return t.value.s.ptr;
    case BIJOU_TBOOLEAN:
        return t.value.b ? "true" : "false";
    case BIJOU_TFUNCTION: {
        char *ca = B_MALLOC(sizeof(char) * 20);
        memset(ca, '\0', 20);
        sprintf(ca, "%p", (void *)&t.value );
        return ca;
    }
    default:
        return "Don't know type";
    }
}

/* return a BijouString representation of t */
BijouString TValue_to_BijouString(TValue t)
{
    return BijouString_new(TValue_to_string(t));
}

/* return a string representation of TValue t */
char *TValue_type_to_string(TValue t)
{
    switch (t.tt) {
    case BIJOU_TNONE:
        return "NONE";
    case BIJOU_TNULL:
        return "null";
    case BIJOU_TNUMBER:
        return "number";
    case BIJOU_TBOOLEAN:
        return "bool";
    case BIJOU_TSTRING:
        return "string";
    case BIJOU_TFUNCTION:
        return "function";
    default:
        return "I haven't a damn clue.";
    }
}
