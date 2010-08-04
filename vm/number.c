/*
TValue TValue_num_add(TValue, TValue);
TValue TValue_num_sub(TValue, TValue);
TValue TValue_num_mul(TValue, TValue);
TValue TValue_num_div(TValue, TValue);
TValue TValue_num_pow(TValue, TValue);
TValue TValue_num_rem(TValue, TValue);
TValue TValue_num_unm(TValue);
*/

#include "bijou.h"
#include "internal.h"
#include "vm.h"
#include "bopcodes.h"

#include <math.h>

TValue TValue_num_add(TValue one, TValue two)
{
    printf("%s + %s  ", TValue_to_string(one), TValue_to_string(two));
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
    printf("%s - %s  ", TValue_to_string(one), TValue_to_string(two));
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
    printf("%s * %s  ", TValue_to_string(one), TValue_to_string(two));
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
    printf("%s ** %s  ", TValue_to_string(one), TValue_to_string(two));
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
    printf("%s / %s  ", TValue_to_string(one), TValue_to_string(two));
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
    printf("%s %% %s  ", TValue_to_string(one), TValue_to_string(two));
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
    printf("%ld  ", cast(long, one.value.n) % cast(long, two.value.n));
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
