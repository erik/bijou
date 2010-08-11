
#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "bopcodes.h"

#include <math.h>

TValue TValue_num_add(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TNUMBER;

    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to add, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }

    result.value = one.value;
    result.value.n += two.value.n;
    return result;
}

TValue TValue_num_sub(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TNUMBER;

    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to sub, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }

    result.value = one.value;
    result.value.n -= two.value.n;
    return result;
}

TValue TValue_num_mul(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TNUMBER;

    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to mul, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }

    result.value = one.value;
    result.value.n *= two.value.n;
    return result;
}

TValue TValue_num_pow(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TNUMBER;
    if ((!ttisnumber(&one)) || (!ttisnumber(&two))) {
        fprintf(stderr, "ERROR: expected two number arguments to pow, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }

    result.value = one.value;
    result.value.n = pow(result.value.n, two.value.n);
    return result;
}

TValue TValue_num_div(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TNUMBER;

    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to div, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }

    result.value = one.value;
    result.value.n /= two.value.n;
    return result;
}

TValue TValue_num_rem(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TNUMBER;

    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to rem, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }

    result.value = one.value;
    result.value.n = (long long )one.value.n % (long long)two.value.n;
    return result;
}

TValue TValue_num_unm(TValue t)
{
    if (! ttisnumber(&t)) {
        fprintf(stderr, "ERROR: expected a number argument for unm, but got [%s]\n", TValue_type_to_string(t));
        return (t.tt = BIJOU_TNONE, t);
    }
    return (t.value.n = -t.value.n, t);
}

TValue TValue_num_lt(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TBOOLEAN;
    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to lt, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }
    result.value = create_boolean(one.value.n < two.value.n).value;
    return result;
}

TValue TValue_num_le(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TBOOLEAN;
    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to le, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }
    result.value = create_boolean(one.value.n <= two.value.n).value;
    return result;
}

TValue TValue_num_gt(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TBOOLEAN;
    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to gt, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }
    result.value = create_boolean(one.value.n > two.value.n).value;
    return result;
}

TValue TValue_num_ge(TValue one, TValue two)
{
    TValue result;
    result.tt = BIJOU_TBOOLEAN;
    if (! ttisnumber(&one) || ! ttisnumber(&two)) {
        fprintf(stderr, "ERROR: expected two number arguments to ge, but got [%s] and [%s]\n",
                TValue_type_to_string(one), TValue_type_to_string(two));
        result.tt = BIJOU_TNONE;
        return result;
    }
    result.value = create_boolean(one.value.n >= two.value.n).value;
    return result;
}
