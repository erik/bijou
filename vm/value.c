/* TValue functions */

#include "bijou.h"
#include "internal.h"
#include <string.h>

/* create a TValue from a number */
TValue create_bijou_Number(bijou_Number num){
  Value v;
  v.n = num;
  TValue ret;
  ret.tt = BIJOU_TNUMBER;
  ret.value = v;
  return ret;
}

/* returns nonzero if the two parameters are equal,
 * zero otherwise. And yes, it is impossible to decipher
 */ 
int TValue_equal(TValue f, TValue s) {
  /* lol, ternary */
  return f.tt == s.tt ? f.tt == BIJOU_TNULL ? 1 : \
    f.tt == BIJOU_TNUMBER || f.tt == BIJOU_TBOOLEAN \
    ? (f.value.n == s.value.n) : f.tt == BIJOU_TSTRING ? \
    strcmp(f.value.s.ptr, s.value.s.ptr) == 0 : &f == &s : 0;
}
